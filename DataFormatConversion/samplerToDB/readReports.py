#!/s/python-2.5/bin/python -O
# -*- python -*-

from __future__ import with_statement

import optparse
import glob
import re
import sqlite3
import sys

from itertools import count
from os.path import basename, join
from collections import defaultdict

from DBConstants import EnumerationTables
from utils import InsertQueryConstructor

reportOpen = re.compile(r'<report id="([a-zA-Z-]+)">')
reportClose = re.compile(r'</report>')
samplesOpen = re.compile(r'<samples unit="([0-9a-f]{32})" scheme="([a-zA-Z-]+)"(?: phase="([0-9]+)")?(?: threadid="([0-9]+)")?>')
samplesClose = re.compile(r'</samples>')

class SampleInfo(object):

    __slots__ = ['signature', 'scheme', 'phase', 'thread']

    def __init__(self, signature, scheme, phase, thread):
        self.signature = signature
        self.scheme = scheme
        self.phase = phase if phase else -1
        self.thread = thread if thread else -1

    def __str__(self):
        return '%s %s %d %d' % (self.signature, self.scheme, self.phase, self.thread)


def readReport(inputs):
    # start reading
    state = 'INITIAL'

    for line in inputs:
        line = line.strip()

        isReportOpen, isReportClose = None, None
        isSamplesOpen, isSamplesClose = None, None
        if line[0] == '<':
            isReportOpen = reportOpen.match(line)
            isReportClose = reportClose.match(line)
            isSamplesOpen = samplesOpen.match(line)
            isSamplesClose = samplesClose.match(line)

        if state == 'INITIAL':
            if isReportOpen is None:
                raise ValueError('Read "%s" instead of <report> tag' % line)
            if isReportOpen.group(1) == 'samples':
                state = 'REPORT'
            else:
                state = 'DISCARD'

        elif state == 'DISCARD':
            if isReportOpen or isSamplesOpen or isSamplesClose:
                raise ValueError('Read invalid line "%s" inside '
                                 'a (non-samples) <report> tag' % line)
            elif isReportClose:
                state = 'INITIAL'

        elif state == 'REPORT':
            if isReportClose:
                state = 'INITIAL'
            elif isSamplesOpen:
                yield 'SampleInfo', SampleInfo(*isSamplesOpen.groups())
                state = 'READING'
            else:
                raise ValueError('Read "%s" in a samples subreport' % line)

        elif state == 'READING':
            if isSamplesClose:
                state = 'REPORT'
            else:
                yield 'CountInfo', line.split('\t')

    if state != 'INITIAL':
        raise ValueError('Report reading terminated in state ' + state)


def getUnitInfo(conn):
    """ Return [list-of-siteIDs] for all units as a
        dictionary indexed by (Signature, SchemeID)

        Assumes that siteIDs of sites in a unit are contiguous.
    """

    # Performance tip: Create an index on Sites based on UnitID
    query = 'SELECT Signature, SchemeID, MIN(SiteID), COUNT(SiteID)\
             FROM Units JOIN Sites ON Units.UnitID=Sites.UnitID\
             GROUP BY Units.UnitID'

    result = defaultdict(list)
    cursor = conn.cursor()
    for row in cursor.execute(query):
        # NOTE The cast to 'str' below is required because
        # sqlite3 returns unicode strings by default.
        # Another option is to setconn.text_factory to 'str'.
        key = (str(row[0]), row[1])
        value = xrange(row[2], row[2] + row[3])

        result[key] = value
    cursor.close()

    return result

def processReportFile(cursor, UnitInfoMap, runID, fname, wantedSchemes=None):
    SchemeNameToID = dict((t[1], t[0]) for t in EnumerationTables['Schemes'])
    qg = InsertQueryConstructor()

    if wantedSchemes is None:
        wantedSchemes = SchemeNameToID.keys()

    def ensureAllCountersAreRead(sample, counter):
        try:
            want = counter.next()
        except StopIteration:
            return

        # Some counter hasn't yet been read.
        raise ValueError('Samples in unit "%s" ended before counter '
                         '%d could be read' % (str(sample), want))

    with file(fname) as ifile:
        curSampleInfo = None
        curSchemeID = None
        curPhase = None
        curThread = None
        siteIDCounter = None
        isFilteredScheme = False
        insertQuery = None
        values = None
        curFieldIDs = None
        ignoreZeros = False
        # store the keys for the observed units, to accommodate the use case 
        # for shared libraries where the same unit signature and schemeid 
        # may be observed multiple times. In such a scenario we are supposed 
        # to add the counts together (instead of simply inserting records in 
        # the database we need to update the pre-existing ones)
        unitsObserved = {}
        isSharedLibrary = False
        updateQuery = None

        for kind, info in readReport(ifile):
            if kind == 'SampleInfo':
                if siteIDCounter:
                    ensureAllCountersAreRead(curSampleInfo, siteIDCounter)
                    if isSharedLibrary:
                        cursor.executemany(updateQuery, values)
                    cursor.executemany(insertQuery, values)

                if info.scheme not in wantedSchemes:
                    isFilteredScheme = True
                    continue
                else:
                    isFilteredScheme = False

                curSampleInfo = info
                curSchemeID = SchemeNameToID[info.scheme]
                curPhase = info.phase
                curThread = info.thread
                key = (info.signature, curSchemeID)
                siteIDCounter = iter(UnitInfoMap[key])
                insertQuery, curFieldIDs, ignoreZeros = qg.generateInsertionScenario(curSchemeID)
                if unitsObserved.has_key(key):
                    isSharedLibrary = True
                    updateQuery = qg.generateUpdateQuery(curSchemeID) 
                else:
                    unitsObserved[key] = None
                    isSharedLibrary = False
                    updateQuery = None 
                values = []

            elif isFilteredScheme:
                continue

            elif kind == 'CountInfo':
                try:
                    siteID = siteIDCounter.next()
                except StopIteration:
                    raise ValueError('Found too many samples while reading '
                                     'unit "%s".' % str(curSampleInfo))

                if len(curFieldIDs) != len(info):
                    raise ValueError('Got %d samples while expecting %d samples '
                                      'for site %d with scheme %d'
                                      % (len(samples), len(fields), siteID, schemeID))

                for sample, fieldID in zip(info, curFieldIDs):
                    sample = int(sample)
                    if ignoreZeros and sample == 0:
                        continue
                    values.append((sample, curPhase, runID, curThread, siteID, fieldID))

        if siteIDCounter:
            ensureAllCountersAreRead(curSampleInfo, siteIDCounter)
        if len(values) > 0:
            if isSharedLibrary:
                cursor.executemany(updateQuery, values)
            cursor.executemany(insertQuery, values)


def processReports(conn, runDirs, schemes=None, version=1):
    """ Arguments:
            conn: A database connection
            runDirs: A list (or iterator) that generates run directories
                     to be processed
            schemes: A list of schemes that are enabled during analysis
    """
    if version != 1:
        raise ValueError('Version %s of the database schema is unsupported' %
                         str(version))

    outcomeEnum = EnumerationTables['Outcomes']
    UNKNOWN = [t[0] for t in outcomeEnum if t[1] == 'UNKNOWN'][0]

    testCaseMap = {}
    for row in conn.execute('SELECT TestCase, RunID from RunIDToTestCase'):
        testCaseMap[row[0]] = row[1]

    UnitInfoMap = getUnitInfo(conn)
    cursor = conn.cursor()
    for runDir in runDirs:
        testCase = int(basename(runDir))
        if testCase not in testCaseMap:
            continue
        runID = testCaseMap[testCase]

        reportFile = join(runDir, 'reports')
        try:
            processReportFile(cursor, UnitInfoMap, runID, reportFile, wantedSchemes=schemes)
        except ValueError, ve:
            print 'Following error while reading report file %s.\n\t%s' % (reportFile, ve)
            raise

    cursor.execute('CREATE INDEX IndexSampleCountsByRunID ON SampleCounts(RunID)')
    cursor.execute('CREATE INDEX IndexSampleValuesByRunID ON SampleValues(RunID)')
    # These analyze commands are specific to SQLite and are needed so that
    # the correct index (there is a second latent index due to primary keys) 
    # is chosen when querying the tables.
    # TAGS: SQLITE_SPECIFIC, PERFORMANCE
    cursor.execute('ANALYZE SampleCounts;')
    cursor.execute('ANALYZE SampleValues;')
    conn.commit()

def main():
    """ Read reports in <tests-dir> and insert counts into <database>.
        Assume that Runs, Sites and Units are already inserted
    """

    parser = optparse.OptionParser(usage='%prog [options] <database> <tests-dir>')
    parser.add_option('-s', '--scheme', action='append', dest='schemes',
                      choices=[t[1] for t in EnumerationTables['Schemes']],
                      help = 'add sites with scheme SCHEME.  All schemes\
                              will be processed if this flag is omitted')
    parser.add_option('-v', '--version', action='store', default=1, type='int',
                      help = 'version of schema to implement')

    options, args = parser.parse_args()
    if len(args) != 2:
        parser.error('wrong number of positional arguments')
    if options.version != 1:
        parser.error('CBI database schema version ' + options.version +
                     ' is currently not supported')

    cbi_db, testsDir = args
    runDirs = glob.iglob(join(testsDir, '[0-9]*/[0-9]*'))

    conn = sqlite3.connect(cbi_db)
    processReports(conn, runDirs, options.version, options.schemes)
    conn.close()


################################################################################

if __name__ == '__main__':
    main()

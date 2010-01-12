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

from DBConstants import EnumerationTables
from utils import InsertQueryConstructor

# regular expressions
reportOpen = re.compile(r'<report id="([a-zA-Z-]+)">')
reportClose = re.compile(r'</report>')
samplesOpen = re.compile(r'<samples unit="([0-9a-f]{32})" scheme="([a-zA-Z-]+)">')
samplesClose = re.compile(r'</samples>')

class SampleInfo(object):

    __slots__ = ['signature', 'scheme']

    def __init__(self, signature, scheme):
        self.signature = signature
        self.scheme = scheme

    def __str__(self):
        return '%s %s' % (self.signature, self.scheme)


class CountInfo(object):

    __slots__ = ['counts']

    def __init__(self, counts):
        self.counts = counts


def readReport(inputs):
    # start reading
    state = 'INITIAL'

    for line in inputs:
        line = line.strip()

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
                yield SampleInfo(*isSamplesOpen.groups())
                state = 'READING'
            else:
                raise ValueError('Read "%s" in a samples subreport' % line)

        elif state == 'READING':
            if isSamplesClose:
                state = 'REPORT'
            else:
                yield CountInfo(line.split('\t'))

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

    result = {}
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

def processReportFile(conn, runID, fname, phase=-1, wantedSchemes=None):
    UnitInfoMap = getUnitInfo(conn)
    SchemeNameToID = dict((t[1], t[0]) for t in EnumerationTables['Schemes'])
    qg = InsertQueryConstructor()

    if wantedSchemes is None:
        wantedSchemes = SchemeNameToID.keys()

    cursor = conn.cursor()

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
        siteIDCounter = None
        isFilteredScheme = False

        for info in readReport(ifile):
            if isinstance(info, SampleInfo):
                if siteIDCounter:
                    ensureAllCountersAreRead(curSampleInfo, siteIDCounter)

                if info.scheme not in wantedSchemes:
                    isFilteredScheme = True
                    continue
                else:
                    isFilteredScheme = False

                curSampleInfo = info
                curSchemeID = SchemeNameToID[info.scheme]
                key = (info.signature, curSchemeID)
                siteIDCounter = iter(UnitInfoMap[key])

            elif isFilteredScheme:
                continue

            elif isinstance(info, CountInfo):
                try:
                    siteID = siteIDCounter.next()
                except StopIteration:
                    raise ValueError('Found too many samples while reading '
                                     'unit "%s".' % str(curSampleInfo))

                queries = qg.generateQueries(siteID, runID, curSchemeID,
                                             info.counts, phase)
                for query, values in queries:
                    cursor.execute(query, values)

        if siteIDCounter:
            ensureAllCountersAreRead(curSampleInfo, siteIDCounter)


def processReports(conn, runDirs, version, schemes=None, filtered=False):
    """ Arguments:
            conn: A database connection
            runDirs: A list (or iterator) that generates run directories
                     to be processed
    """

    outcomeEnum = EnumerationTables['Outcomes']
    UNKNOWN = [t[0] for t in outcomeEnum if t[1] == 'UNKNOWN'][0]

    ignoredRuns = set()
    for row in conn.execute('SELECT RunID, OutcomeID FROM Runs'):
        if row[1] == UNKNOWN:
            ignoredRuns.insert(row[0])

    for runDir in runDirs:
        runID = int(basename(runDir))
        if filtered and runID in ignoredRuns:
            continue

        reportFile = join(runDir, 'reports')
        try:
            processReportFile(conn, runID, reportFile, wantedSchemes=schemes)
        except ValueError, ve:
            print 'Following error while reading report file %s.\n\t%s' % (reportFile, ve)
            raise

    conn.execute('CREATE INDEX IndexByRunID ON SampleCounts(RunID)')
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

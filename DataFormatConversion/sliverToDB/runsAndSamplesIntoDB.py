#!/s/python-2.5/bin/python -O
# -*- python -*-

from __future__ import with_statement

import sqlite3
import sys
import optparse
import os

from collections import defaultdict
from itertools import count, izip, imap

from initializeSchema import EnumerationTables

###############################################################################
# Insert runs and outcomes from outcomes.txt
###############################################################################

def writeRunsIntoDB(conn, outcomesTxt, version):
    """ Populate Runs table from file 'outcomesTxt'.  The file
        contains one integer per line, representing the outcome
        of runs, starting at index 0.
    """

    def getOutcome(line):
        outcome = int(line.strip())
        if outcome == 0:
            return 0
        else:
            return 1

    runID = count()
    values = izip(runID, imap(getOutcome, file(outcomesTxt)))

    cursor = conn.cursor()
    cursor.executemany('INSERT INTO Runs VALUES (?,?)', values)
    cursor.close()
    conn.commit()


###############################################################################
# Insert samples from counts.txt
###############################################################################

ValueRecordingSchemes = frozenset(('bounds',))

def getSchemeIDForSite(siteID, cursor, siteIDToSchemeCache = {}):
    """Get the SchemeID corresponding to a particular Site.

        ARGUMENTS:
            siteID: Non-negative integer corresponding to the ID of the site
            cursor: sqlite3 cursor object
            siteIDToSchemeCache: Used to cache the results of queries. Do NOT
                                assign it a value when calling method.
    """
    if siteID in siteIDToSchemeCache:
        return siteIDToSchemeCache[siteID]
    else:
        query = 'SELECT SchemeID FROM SITES WHERE SiteID=?'
        cursor.execute(query, (siteID,))
        r = cursor.fetchone()
        siteIDToSchemeCache[siteID] = r[0]
        return r[0]



def writeSamplesIntoDB(conn, countsTxt, phase, version):
    """Input sample counts/values into the database.

        ARGUMENTS:
            conn: sqlite3 connection object
            countsTxt: Path to the counts.txt file
            phase: The phase number (default= -1) corrsponding to the data
                    being recorded for the given set of runs
            version: Version of the schema supported. Currently only supports
                    version 1.
    """

    def getSchemeIDToFieldMapping():
        R = defaultdict(list)
        for t in EnumerationTables['Fields']:
            R[t[1]].append(t[0])

        return R

    def getSchemeIDToTableMapping():
        R = {}
        for t in  EnumerationTables['Schemes']:
            if t[1] in ValueRecordingSchemes:
                R[t[0]] = 'SampleValues'
            else:
                R[t[0]] = 'SampleCounts'

        return R


    def inputSamplesForSingleRun(conn, runID, samplesForRun):
        """Input sample counts/values for a single run.

            ARGUMENTS:
                conn: sqlite3 connection object
                runID: The runID (starting with 0) for which the counts will be
                        inserted into the database
                samplesForRun: str object corresponding to one line from the
                                counts.txt file
        """
        c = conn.cursor()

        for site in samplesForRun.strip().split('\t'):
            samples = map(int, site.split(':'))
            siteID = samples[0]
            samples = samples[1:]
            schemeID = getSchemeIDForSite(siteID, c)
            table = SchemeIDToTable[schemeID]
            entries = ', '.join('?' * 5)
            query = 'INSERT INTO %s VALUES (%s);' % (table, entries)
            if table == 'SampleValues':
                for sample, fieldID in zip(samples, SchemeIDToFields[schemeID]):
                    c.execute(query, (siteID, fieldID, runID, sample, phase))
            elif table == 'SampleCounts':
                for sample, fieldID in zip(samples, SchemeIDToFields[schemeID]):
                    if sample != 0:
                        c.execute(query, (siteID,fieldID,runID,sample,phase))

        c.close()


    SchemeIDToFields = getSchemeIDToFieldMapping()
    SchemeIDToTable = getSchemeIDToTableMapping()
    runID = count()

    with file(countsTxt, 'r') as countsFile:
        for line in countsFile:
            inputSamplesForSingleRun(conn, runID.next(), line)

    conn.execute('CREATE INDEX IndexByRunID ON SampleCounts(RunID)')
    conn.commit()


def main(argv=None):
    """ Insert run outcomes and sampled counts/values
        into appropriate tables.
    """

    if argv is None:
        argv = sys.argv

    parser = optparse.OptionParser(usage='%prog [options] <counts.txt-file> <outcomes.txt-file> <database>.')
    parser.add_option('-v', '--version', action='store', default=1,
                      help = 'version of schema to implement')
    parser.add_option('-p', '--phase', action='store', default=-1,
                        help = 'phase number associated with runs')

    options, args = parser.parse_args(argv[1:])
    if len(args) != 3:
        parser.error('wrong number of positional arguments')

    countsTxt, outcomesTxt, cbi_db = args

    def checkExists(kind, name):
        if not os.path.exists(name):
            parser.error('%s %s does not exist' % (kind, name))

    checkExists('counts-file', countsTxt)
    checkExists('outcomes-file', outcomesTxt)
    checkExists('database', cbi_db)

    if options.version != 1:
        parser.error('CBI database schema version ' + options.version +
                     ' is currently not supported')

    conn = sqlite3.connect(cbi_db)

    writeRunsIntoDB(conn, outcomesTxt, options.version)
    writeSamplesIntoDB(conn, countsTxt, options.phase, options.version)

    conn.close()


if __name__ == '__main__':
    sys.exit(main())

#!/s/python-2.5/bin/python -O
# -*- python -*-

from __future__ import with_statement

import sqlite3
import sys
import optparse
import os

from collections import defaultdict
from itertools import count

from initializeSchema import EnumerationTables


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



def inputSamples(conn, countsTxt, phase, version):
    """Input sample counts/values into the database.

        ARGUMENTS:
            conn: sqlite3 connection object
            countsTxt: Path to the counts.txt file
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

    conn.commit()


def main(argv=None):
    """Insert sampled counts/values into appropriate tables.
    """

    if argv is None:
        argv = sys.argv

    parser = optparse.OptionParser(usage='%prog [options] <database>.')
    parser.add_option('-f', '--counts-file', action='store', default=None,
                        help = 'path to the counts.txt file')
    parser.add_option('-v', '--version', action='store', default=1,
                      help = 'version of schema to implement')
    parser.add_option('-p', '--phase', action='store', default=-1,
                        help = 'phase number associated with runs')

    options, args = parser.parse_args(argv[1:])
    if len(args) != 1: parser.error('wrong number of positional arguments')

    cbi_db = args[0]
    if os.path.exists(cbi_db):
        if options.version != 1:
            return ('CBI database schema version ' + options.version +
                    ' is currently not supported')
        else:
            if os.path.exists(options.counts_file):
                conn = sqlite3.connect(cbi_db)
            else:
                return 'Make sure the path to counts-file is correct'
    else:
        return 'Make sure the path to database file is correct'

    inputSamples(conn, options.counts_file, options.phase, options.version)


if __name__ == '__main__':
    sys.exit(main())

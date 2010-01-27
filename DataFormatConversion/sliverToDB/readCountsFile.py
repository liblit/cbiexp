#!/s/python-2.5/bin/python -O
# -*- python -*-

from __future__ import with_statement

import sqlite3
import sys
import optparse
import os

from itertools import count

from DBConstants import EnumerationTables
from utils import getSiteIDToSchemeMapping, InsertQueryConstructor

def processCountsFile(conn, countsTxt, phase, version):
    """Input sample counts/values into the database.

        ARGUMENTS:
            conn: sqlite3 connection object
            countsTxt: Path to the counts.txt file
            phase: The phase number (default= -1) corrsponding to the data
                    being recorded for the given set of runs
            version: Version of the schema supported. Currently only supports
                    version 1.
    """

    def inputSamplesForSingleRun(cursor, runID, samplesForRun):
        """Input sample counts/values for a single run.

            ARGUMENTS:
                conn: sqlite3 connection object
                runID: The runID (starting with 0) for which the counts will be
                        inserted into the database
                samplesForRun: str object corresponding to one line from the
                                counts.txt file
        """
        for site in samplesForRun.strip().split('\t'):
            samples = map(int, site.split(':'))
            siteID = samples[0]
            samples = samples[1:]
            schemeID = SiteIDToSchemeID[siteID]

            query = qg.generateQuery(schemeID)
            values = qg.generateQueries(siteID, runID, schemeID,
                                         samples, phase)
            cursor.executemany(query, values)

    SiteIDToSchemeID = getSiteIDToSchemeMapping(conn)
    qg = InsertQueryConstructor()
    runID = count()

    cursor = conn.cursor()
    with file(countsTxt, 'r') as countsFile:
        for line in countsFile:
            inputSamplesForSingleRun(cursor, runID.next(), line)

    cursor.execute('CREATE INDEX IndexSampleCountsByRunID ON SampleCounts(RunID)')
    cursor.execute('CREATE INDEX IndexSampleValuesByRunID ON SampleValues(RunID)')
    conn.commit()


def main(argv=None):
    """ Insert run outcomes and sampled counts/values
        into appropriate tables.
    """

    if argv is None:
        argv = sys.argv

    parser = optparse.OptionParser(usage='%prog [options] <database> <counts.txt-file>')
    parser.add_option('-v', '--version', action='store', default=1, type='int',
                      help = 'version of schema to implement')
    parser.add_option('-p', '--phase', action='store', default=-1, type='int',
                        help = 'phase number associated with runs')

    options, args = parser.parse_args(argv[1:])
    if len(args) != 2:
        parser.error('wrong number of positional arguments')

    cbi_db, countsTxt = args

    def checkExists(kind, name):
        if not os.path.exists(name):
            parser.error('%s %s does not exist' % (kind, name))

    checkExists('counts-file', countsTxt)
    checkExists('database', cbi_db)

    if options.version != 1:
        parser.error('CBI database schema version ' + options.version +
                     ' is currently not supported')

    conn = sqlite3.connect(cbi_db)
    processCountsFile(conn, countsTxt, options.phase, options.version)
    conn.close()


if __name__ == '__main__':
    sys.exit(main())

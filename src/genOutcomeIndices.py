#!/s/python-2.5/bin/python
from __future__ import with_statement

import sqlite3

from optparse import OptionParser
from os.path import exists

def queryOutcomeEnum(conn):
    """retrieve enumerations from the 'Outcomes' table into a dict"""

    cursor = conn.cursor()
    cursor.execute('SELECT Name, OutcomeID from Outcomes')
    OutcomeIDs = dict(iter(cursor))
    cursor.close()

    return OutcomeIDs

def writeRunIDs(conn, outcome, output):
    """write RunIDs with a particular <outcome> to <output>"""

    query = 'SELECT RunID from Runs WHERE OutcomeID=? ORDER BY RunID'
    cursor = conn.cursor()
    cursor.execute(query, (outcome,))

    with file(output, 'w') as ofile:
        for row in cursor:
            print >>ofile, row[0]

    cursor.close()

def extractRunOutcomes(cbi_db, fFile, sFile, outcomesFile):
    """ read outcomes from database and write
            1. IDs of failing runs to <fFile>
            2. IDs of successful runs to <sFile>
            3. Vector of run outcomes (0 = success, 1 = failure)
                to <outcomesFile>
    """

    conn = sqlite3.connect(cbi_db)

    OutcomeIDs = queryOutcomeEnum(conn)
    writeRunIDs(conn, OutcomeIDs['SUCCESS'], sFile)
    writeRunIDs(conn, OutcomeIDs['FAILURE'], fFile)

    cursor = conn.cursor()
    with file(outcomesFile, 'w') as ofile:
        for row in cursor.execute('SELECT OutcomeID from Runs ORDER BY RunID'):
            print >>ofile, row[0]

    cursor.close()
    conn.close()

def main():
    parser = OptionParser(usage='%prog <database> <f.runs-file> <s.runs-file> <outcomes-txt>')

    options, args = parser.parse_args()
    if len(args) != 4: parser.error('wrong number of positional arguments')
    if not exists(args[0]):
        parser.error('sqlite database %s does not exist' % args[0])

    extractRunOutcomes(*args)

if __name__ == "__main__":
    main()

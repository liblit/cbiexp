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

def genOutcomeIndices(cbi_db, fFile, sFile):
    """read outcomes from database and generate f.runs + s.runs"""

    if not exists(cbi_db):
        parser.error('sqlite database %s does not exist' % cbi_db)

    conn = sqlite3.connect(cbi_db)

    OutcomeIDs = queryOutcomeEnum(conn)
    writeRunIDs(conn, OutcomeIDs['SUCCESS'], sFile)
    writeRunIDs(conn, OutcomeIDs['FAILURE'], fFile)
    conn.close()

def main():
    parser = OptionParser(usage='%prog <database> <f.runs-file> <s.runs-file>')
    options, args = parser.parse_args()
    if len(args) != 3: parser.error('wrong number of positional arguments')

    genOutcomeIndices(*args)

if __name__ == "__main__":
    main()

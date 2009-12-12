#!/s/python-2.5/bin/python -O
# -*- python -*-

import sqlite3
import sys
import optparse
import os

from itertools import count, izip, imap

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


def main(argv=None):
    """ Insert runs and their outcomes into the Runs table. """

    if argv is None:
        argv = sys.argv

    parser = optparse.OptionParser(usage='%prog [options] <database> <outcomes.txt-file>')
    parser.add_option('-v', '--version', action='store', default=1,
                      help = 'version of schema to implement')

    options, args = parser.parse_args(argv[1:])
    if len(args) != 2:
        parser.error('wrong number of positional arguments')

    cbi_db, outcomesTxt = args

    def checkExists(kind, name):
        if not os.path.exists(name):
            parser.error('%s %s does not exist' % (kind, name))

    checkExists('outcomes-file', outcomesTxt)
    checkExists('database', cbi_db)

    if options.version != 1:
        parser.error('CBI database schema version ' + options.version +
                     ' is currently not supported')

    conn = sqlite3.connect(cbi_db)
    writeRunsIntoDB(conn, outcomesTxt, options.version)
    conn.close()


if __name__ == '__main__':
    sys.exit(main())

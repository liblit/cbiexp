#!/s/python-2.5/bin/python -O
# -*- python -*-

import glob
import optparse
import sqlite3
import sys

from itertools import count, izip, imap
from os.path import basename, exists, join

def processOutcomesFile(conn, outcomesTxt, version):
    """ Populate Runs table from file 'outcomesTxt'.  The file
        contains one integer per line, representing the outcome
        of runs, starting at index 0.
    """

    def getOutcome(line):
        outcome = int(line.strip())
        # Outcomes file has -1 for 'IGNORE'
        return {0: 0, 1: 1, -1: 2}[outcome]

    runID = count()
    values = izip(runID, imap(getOutcome, file(outcomesTxt)))

    cursor = conn.cursor()
    cursor.executemany('INSERT INTO Runs VALUES (?,?)', values)
    cursor.close()
    conn.commit()


def processLabels(conn, runDirs, version):
    """ Populate Runs table by iterating over run directories
        and reading 'label' files.
    """

    def getOutcome(line):
        outcome = line.strip()
        return {'success': 0, 'failure': 1, 'ignore': 2}[outcome]

    cursor = conn.cursor()
    for runDir in runDirs:
        try:
            runID = int(basename(runDir))
            labelFile = join(runDir, 'label')
            label = getOutcome(file(labelFile).read())
            cursor.execute('INSERT INTO Runs VALUES (?,?)', (runID, label))
        except Exception, e:
            print 'Following error while reading runDir %s.\n\t%s' % (runDir, e)
            raise

    conn.commit()
    cursor.close()


def main():
    """ Insert runs and their outcomes into the Runs table. """

    parser = optparse.OptionParser(usage='%prog [options] <database>')
    parser.add_option('-o', '--outcomes-file', action='store', type='str',
                      help='file containing outcomes of runs')
    parser.add_option('-r', '--runs-dir', action='store', type='str',
                      help='CBI runs directory')
    parser.add_option('-v', '--version', action='store', default=1, type='int',
                      help='version of schema to implement')

    options, args = parser.parse_args()
    if len(args) != 1:
        parser.error('wrong number of positional arguments')

    if options.version != 1:
        parser.error('CBI database schema version ' + options.version +
                     ' is currently not supported')

    cbi_db = args[0]
    outcomesTxt = options.outcomes_file
    runsDir = options.runs_dir

    def checkExists(kind, name):
        if not exists(name):
            parser.error('%s %s does not exist' % (kind, name))

    checkExists('database', cbi_db)
    conn = sqlite3.connect(cbi_db)

    if outcomesTxt:
        if runsDir:
            parser.error('set exactly one of --outcomes-file and --run-dirs')

        checkExists('outcomes-file', outcomesTxt)
        processOutcomesFile(conn, outcomesTxt, options.version)
    elif runsDir:
        checkExists('runs-dir', runsDir)
        dirs = glob.iglob(join(runsDir, '[0-9]*/[0-9]*'))
        processLabels(conn, dirs, options.version)
    else:
        parser.error('must set either --outcomes-file or --run-dirs')

    conn.close()


if __name__ == '__main__':
    sys.exit(main())

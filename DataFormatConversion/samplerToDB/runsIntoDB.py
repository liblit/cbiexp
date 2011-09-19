#!/s/python-2.7.1/bin/python -O
# -*- python -*-

import optparse
import sqlite3
import sys

from glob import iglob
from itertools import count, izip, imap
from os.path import basename, exists, join

def processOutcomesFile(conn, outcomesTxt, version=1):
    """ Populate Runs table from file 'outcomesTxt'.  The file
        contains one integer per line, representing the outcome
        of runs, starting at index 0.
    """
    if version != 1:
        raise ValueError('Version %s of the database schema is unsupported' %
                         str(version))

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


def processLabels(conn, runDirs, version=1):
    """ Populate Runs table by iterating over run directories
        and reading 'label' files.
    """
    if version != 1:
        raise ValueError('Version %s of the database schema is unsupported' %
                         str(version))

    cursor = conn.cursor()
    runIDCounter = count()
    testCaseMap = []

    for runDir in runDirs:
        try:
            labelFile = join(runDir, 'label')
            label = file(labelFile).read().strip()
            if label == 'ignore':
                continue

            label = {'success': 0, 'failure': 1}[label]
            runID = runIDCounter.next()
            cursor.execute('INSERT INTO Runs VALUES (?,?)', (runID, label))

            testCase = int(basename(runDir))
            testCaseMap.append((runID, testCase))

        except Exception, e:
            print 'Following error while reading runDir %s.\n\t%s' % (runDir, e)
            raise

    conn.execute("""CREATE TABLE
                        RunIDToTestCase(
                            RunID INTEGER NOT NULL
                                CONSTRAINT RunIDToTestCase_pk PRIMARY KEY,
                            TestCase INTEGER NOT NULL UNIQUE,
                            CONSTRAINT RunIDToTestCase_RunID_fk FOREIGN KEY
                                (RunID) REFERENCES Runs(RunID));
                """)

    conn.executemany('INSERT INTO RunIDToTestCase VALUES (?,?)', testCaseMap)

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
        dirs = iglob(join(runsDir, '[0-9]*/[0-9]*'))
        dirs = sorted(dirs, key=lambda d: int(basename(d)))
        processLabels(conn, dirs, options.version)
    else:
        parser.error('must set either --outcomes-file or --run-dirs')

    conn.close()


if __name__ == '__main__':
    sys.exit(main())

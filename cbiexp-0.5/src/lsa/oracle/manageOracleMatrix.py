#!/s/std/bin/python
# -*- python -*-

from optparse import OptionParser
from os.path import join
from subprocess import call

def main():
    parser = OptionParser(usage='%prog numrestarts') 

    parser.add_option('-r', '--remove', action='store_true', default=False, help = 'remove hard links to oracle file', dest='action')

    parser.add_option('-a', '--add', action='store_false', default=False, help = 'add hard links to oracle file', dest='action')

    options, args = parser.parse_args()

    # checking that script was called correctly 
    if len(args) != 1: parser.error('wrong number of positional arguments')

    numrestarts = args[0]

    def handleOracle(restart, remove=False):
        curdir = str(restart)
        if remove:
            call(['rm', join(curdir, 'oracle.mat')]) 
        else:
            call(['ln', 'oracle.mat', join(curdir, 'oracle.mat')]) 

    for restart in range(int(numrestarts)):
        handleOracle(restart, options.action)

    return 0

########################################################################


if __name__ == '__main__':
    main()

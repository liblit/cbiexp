#!/s/std/bin/python
# -*- python -*-

import sys

from optparse import OptionParser
from os import mkdir
from os.path import exists, join

def main():
    parser = OptionParser(usage='%prog numaspects numbugaspects numrestarts') 

    options, args = parser.parse_args()

    # checking that script was called correctly 
    if len(args) != 3: parser.error('wrong number of positional arguments')

    numaspects, numbugaspects, numrestarts = args

    for restart in range(int(numrestarts)):
        curdir = str(restart)
        if exists(curdir):
            print 'skipping %s because it has already been made' % curdir 
        else:
            makeexpdir(curdir, numaspects, numbugaspects)
            makehtmldir(curdir)
            makefile = open(join(curdir, 'GNUmakefile'), 'w')
            print >>makefile, 'numaspects = %s' % numaspects
            print >>makefile, 'numbugaspects = %s' % numbugaspects
            print >>makefile, 'include %s/analysis-rules.mk' % sys.path[0] 
            makefile.close()


########################################################################

def makeexpdir(curdir, numaspects, numbugaspects):
    mkdir(curdir)
    makefile = open(join(curdir, 'GNUmakefile'), 'w')
    print >>makefile, 'numaspects = %s' % numaspects
    print >>makefile, 'numbugaspects = %s' % numbugaspects
    print >>makefile, 'include %s/analysis-rules.mk' % sys.path[0] 
    makefile.close()

def makehtmldir(curdir):
    htmldir = join(curdir,'html')
    mkdir(htmldir)
    makefile=open(join(htmldir, 'GNUmakefile'), 'w')
    print >>makefile, 'include %s/analysis-rules.mk' % sys.path[0]
    makefile.close()

if __name__ == '__main__':
    main()

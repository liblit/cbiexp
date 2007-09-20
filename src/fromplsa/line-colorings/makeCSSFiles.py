#!/s/std/bin/python

import sys

from csv import DictReader
from optparse import OptionParser
from os import walk
from os.path import exists, join, splitext

def getColorEncoding(value):
    val = float(value)
    if val > 5 * (1.0/6):
        return '#ff6666'
    if val > 4 * (1.0/6): 
        return '#ffff66'
    if val > 3 * (1.0/6):
        return '#66ff66'
    if val > 2 * (1.0/6):     
        return '#66ffff'
    if val > (1.0/6): 
        return '#6666ff'
    if val > 0.0:
        return '#ff66ff'
    return '#666666' 

def main():
    parser = OptionParser(usage='%prog [options] srcdir')

    parser.add_option('-f', '--force', action='store_true', default=False, help='overwrite files in current target directory, if exist') 

    options, args = parser.parse_args() 

    if len(args) != 1: parser.error('wrong number of positional arguments')

    srcdir = args[0]

    if not(exists(srcdir)):
        print >>sys.stderr, 'Directory %s not found.' % srcdir 
        sys.exit(1)

    fieldnames = ['lineno', 'score']

    # process raw scores
    for root, dirs, files in walk(srcdir):
        for f in filter((lambda f: f.endswith('.scores')), files): 

            # read from file
            infile = join(root, f)
            instream = open(infile)
            reader = DictReader(instream, fieldnames=fieldnames) 
            entries = list(reader)
            instream.close()

            # write to file 
            (outstem, ext) = splitext(infile)
            outfile = '.'.join([outstem, 'css']) 

            if exists(outfile) and not(options.force): 
                print >>sys.stderr, 'File %s already exists.' % outfile 
                print >>sys.stderr, 'Use --force to override.'
                sys.exit(1)

            outstream = open(outfile, 'w')
            for e in entries:
                color = getColorEncoding(e['score'])
                print >>outstream, '#line%s { background: %s }' % (e['lineno'], color)
            outstream.close()

if __name__ == '__main__':
    main()

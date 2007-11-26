#!/s/std/bin/python

import sys

from csv import DictReader
from optparse import OptionParser
from os import walk
from os.path import exists, join, splitext
from xml.dom.minidom import getDOMImplementation

def main():
    parser = OptionParser(usage='%prog [options] srcdir')

    parser.add_option('-f', '--force', action='store_true', default=False, help='overwrite files in current target directory, if exist') 

    options, args = parser.parse_args() 

    if len(args) != 1: parser.error('wrong number of positional arguments')

    srcdir = args[0]

    if not(exists(srcdir)):
        print >>sys.stderr, 'Directory %s not found.' % srcdir 
        sys.exit(1)

    fieldnames = ['lineno', 'score', 'index']

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
            outfile = '.'.join([outstem, 'exp', 'xml']) 

            if exists(outfile) and not(options.force): 
                print >>sys.stderr, 'File %s already exists.' % outfile 
                print >>sys.stderr, 'Use --force to override.'
                sys.exit(1)

            impl = getDOMImplementation() 
            doc = impl.createDocument(None, 'explanations', None)
            top = doc.documentElement

            for e in entries:
                element = doc.createElement('exp')
                element.setAttribute('lineno', e['lineno'])
                element.setAttribute('index', str(int(e['index']) + 1))
                top.appendChild(element)

            outstream = open(outfile, 'w')
            doc.writexml(outstream)
            outstream.close()

if __name__ == '__main__':
    main()

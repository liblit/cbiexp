#!/s/std/bin/python

import sys

from csv import DictReader, DictWriter
from optparse import OptionParser
from os import close
from os.path import commonprefix, exists, split

def main():
    """Reads a csv file where the first column contains filenames."""
    """Removes the common prefix from each filename."""
    """Writes a new file with the stripped filenames."""
    parser = OptionParser(usage='%prog [options] infile outfile')
    parser.add_option('-f', '--force', action='store_true', default=False, help='overwrite current outfile, if exists')

    # check inputs
    options, args = parser.parse_args() 
    if len(args) != 2: parser.error('wrong number of positional arguments') 

    infile = args[0]
    outfile = args[1]

    if exists(outfile) and not(options.force): 
        print >>sys.stderr, 'Target %s already exists.' % outfile
        print >>sys.stderr, 'Use --force to overwrite.'
        sys.exit(1)

    if not(exists(infile)):
        print >>sys.stderr, 'File %s not found.' % infile 
        sys.exit(1)

    infieldnames = ['filename', 'procname', 'lineno'] 
    outfieldnames = ['filename', 'lineno']

    # read file
    instream = open(infile)
    reader = DictReader(instream, fieldnames=infieldnames)
    entries = list(reader) 
    instream.close()

    # process entries
    fnames = map(lambda d: d['filename'], entries)  
    prefix = commonprefix(fnames)

    # if there is only one file, the common prefix will include the filename  
    # however, in the output we want to preserve the filename
    prefix, tail = split(prefix)

    for e in entries: 
        tails = e['filename'].split(prefix)  
        if not(tails[0] == ''): 
            print >>sys.stderr, 'This prefix is uncommon!'
            sys.exit(1) 
        e['filename'] = (tails[1].split('/'))[1] 

    # print results
    outstream = open(outfile, 'w')
    writer = DictWriter(outstream, outfieldnames, extrasaction='ignore', lineterminator='\n')
    writer.writerows(entries)
    outstream.close()

if __name__ == '__main__':
    main()

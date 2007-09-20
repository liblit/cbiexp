#!/s/std/bin/python

import sys

from csv import DictReader, DictWriter
from optparse import OptionParser
from os.path import exists, isdir, join
    
def insertEntry(table, entry): 
    fname = entry['filename'] 
    if (fname in table):
        table[fname].append(entry)
    else:
        table[fname] = [entry]
        
def main():
    parser = OptionParser(usage='%prog [options] infile targetname')

    parser.add_option('-f', '--force', action='store_true', default=False, help='overwrite files in current target directory, if exist') 
    options, args = parser.parse_args() 

    if len(args) != 2: parser.error('wrong number of positional arguments')

    infile = args[0]
    targetname = args[1]

    if not(exists(infile)):
        print >>sys.stderr, 'File %s not found.' % infile 
        sys.exit(1)

    if not(exists(targetname)) or not(isdir(targetname)):
        print >>sys.stderr, 'Target directory %s not found.' % targetname 
        sys.exit(1)


    # reading from file 
    instream = open(infile) 
    infieldnames = ['filename', 'lineno', 'score']
    reader = DictReader(instream, fieldnames=infieldnames)
    entries = list(reader)
    instream.close()

    # sorting by filename
    ftable = {}
    for e in entries:
        insertEntry(ftable, e) 
    
    # putting this information into separate files in directory tree
    outfieldnames = ['lineno', 'score'] 
    for k in ftable.keys():
        outfile = join(targetname, ".".join([k, 'scores', 'raw']))

        if exists(outfile) and not(options.force):
            print >>sys.stderr, 'File %s already exists.' % outfile
            print >>sys.stderr, 'Use --force to overwrite.' 
            sys.exit(1)

        outstream = open(outfile, 'w')
        writer = DictWriter(outstream, outfieldnames, extrasaction='ignore', lineterminator='\n') 
        writer.writerows(ftable[k])
        outstream.close()

        
if __name__ == '__main__':
    main()

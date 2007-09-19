#!/s/std/bin/python

import sys

from csv import DictReader, DictWriter
from optparse import OptionParser
from os import walk
from os.path import exists, join, splitext

def insertEntry(table, entry):
    lineno = entry['lineno']
    if lineno in table:
        table[lineno].append(entry['score'])
    else:
        table[lineno] = [entry['score']]

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
        for f in filter((lambda f: f.endswith('.scores.raw')), files): 

            # read from file
            infile = join(root, f)
            instream = open(infile)
            reader = DictReader(instream, fieldnames=fieldnames) 
            entries = list(reader)
            instream.close()

            # sort by lineno 
            ltable = {}
            for e in entries:
                insertEntry(ltable, e)

            # average scores for each line no 
            for k in ltable.keys():
                scores = ltable[k] 
                ltable[k] = sum(map(lambda s : float(s), scores))/len(scores)    
            # split into dicts 
            entries = []
            for i in ltable.iteritems(): 
                entries.append({'lineno': i[0], 'score':i[1]})

            # sort, based on numeric value of lineno
            entries.sort(key=(lambda e: int (e['lineno'])))
                
            # make new file        
            (outfile,ext) = splitext(infile)

            if exists(outfile) and not(options.force):
                print >>sys.stderr, 'File %s already exists.' % outfile
                print >>sys.stderr, 'Use --force to overwrite.'
                sys.exit(1)

            outstream = open(outfile, 'w')
            writer = DictWriter(outstream, fieldnames=fieldnames) 
            writer.writerows(entries)
            outstream.close()
        
if __name__ == '__main__':
    main()

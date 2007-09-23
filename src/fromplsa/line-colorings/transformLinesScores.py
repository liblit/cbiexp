#!/s/std/bin/python

import sys

from csv import DictReader, DictWriter
from itertools import count, izip
from optparse import OptionParser
from os import walk
from os.path import exists, join, splitext

def insertEntry(table, entry):
    
    newentry = {'score':entry['score'], 'index':entry['index']}

    lineno = entry['lineno']
    if lineno in table:
        table[lineno].append(newentry)
    else:
        table[lineno] = [newentry]

def main():
    parser = OptionParser(usage='%prog [options] srcdir')

    parser.add_option('-f', '--force', action='store_true', default=False, help='overwrite files in current target directory, if exist') 

    def localMean(scores):
        dummy = -1
        mean = sum(map(lambda s : float(s['score']), scores))/len(scores)    
        return {'score':mean, 'index':dummy}

    def localMax(scores):
        scores.sort(key=lambda s: float(s['score']), reverse=True)
        return scores[0] 

    transfunctions = {'mean':localMean, 'max':localMax} 
    parser.add_option('-t', '--transformation', action='store', default='mean', help='indicate transformation') 

    options, args = parser.parse_args() 

    if len(args) != 1: parser.error('wrong number of positional arguments')
    if options.transformation not in transfunctions.keys(): 
        print >>sys.stderr, '%s is not an allowable transformation.' % options.transformation
        print >>sys.stderr, 'Options are: ', transfunctions.keys() 
        sys.exit(1) 

    srcdir = args[0]

    if not(exists(srcdir)):
        print >>sys.stderr, 'Directory %s not found.' % srcdir 
        sys.exit(1)

    infieldnames = ['lineno', 'score']
    outfieldnames = ['lineno', 'score', 'index']

    # process raw scores
    for root, dirs, files in walk(srcdir):
        for f in filter((lambda f: f.endswith('.scores.raw')), files): 

            # read from file
            infile = join(root, f)
            instream = open(infile)
            reader = DictReader(instream, fieldnames=infieldnames) 
            entries = list(reader)
            instream.close()

            # number the entries, according to their order in the file 
            for e,c in izip(entries, count(0)):
                e['index'] = c

            # insert entries 
            ltable = {}
            for e in entries:
                insertEntry(ltable, e)

            # transform scores
            for k in ltable.keys():
                scores = ltable[k] 
                ltable[k] = transfunctions[options.transformation](scores)

            # split into dicts 
            entries = []
            for i in ltable.iteritems(): 
                newentry = {'lineno':i[0], 'score':i[1]['score'], 'index':i[1]['index']} 
                entries.append(newentry)

            # sort, based on numeric value of lineno
            entries.sort(key=(lambda e: int (e['lineno'])))
                
            # make new file        
            (outfile,ext) = splitext(infile)

            if exists(outfile) and not(options.force):
                print >>sys.stderr, 'File %s already exists.' % outfile
                print >>sys.stderr, 'Use --force to overwrite.'
                sys.exit(1)

            outstream = open(outfile, 'w')
            writer = DictWriter(outstream, fieldnames=outfieldnames) 
            writer.writerows(entries)
            outstream.close()
        
if __name__ == '__main__':
    main()

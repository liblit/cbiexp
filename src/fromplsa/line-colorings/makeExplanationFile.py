#!/s/std/bin/python

import sys

from optparse import OptionParser
from os import P_WAIT, spawnvp, walk
from os.path import exists, join, splitext

def main():
    parser = OptionParser(usage='%prog [options] srcdir analysisdir tooldir parentdir')

    parser.add_option('-f', '--force', action='store_true', default=False, help='overwrite files in current target directory, if exist') 

    options, args = parser.parse_args() 

    if len(args) != 4: parser.error('wrong number of positional arguments')

    srcdir = args[0]
    analysisdir = args[1]
    tooldir = args[2] 
    parentdir = args[3]

    if not(exists(srcdir)):
        print >>sys.stderr, 'Directory %s not found.' % srcdir 
        sys.exit(1)

    # render explanations into html 
    for root, dirs, files in walk(srcdir):
        for f in filter((lambda f: f.endswith('.exp.xml')), files): 

            infile = join(root, f)

            (outstem, ext) = splitext(infile)
            outfile = '.'.join([outstem, 'html']) 

            if exists(outfile) and not(options.force): 
                print >>sys.stderr, 'File %s already exists.' % outfile 
                print >>sys.stderr, 'Use --force to override.'
                sys.exit(1)
            
            functionargs=[] 
            functionargs.append('--verbose')
            functionargs.append('--output')
            functionargs.append(outfile)
            functionargs.append('--path')
            functionargs.append(analysisdir)
            functionargs.append('--path')
            functionargs.append(srcdir)
            functionargs.append('--path')
            functionargs.append(tooldir)
            functionargs.append('--path')
            functionargs.append(parentdir)
            functionargs.append('render-explanations.xsl')
            functionargs.append(infile)
            print >>sys.stderr, functionargs
            spawnvp(P_WAIT, 'xsltproc', functionargs)

if __name__ == '__main__':
    main()

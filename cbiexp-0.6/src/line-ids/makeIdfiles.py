#!/s/std/bin/python

import sys

from optparse import OptionParser
from os import mkdir, remove, walk
from os.path import basename, exists, isdir, join, splitext
from shutil import rmtree
from subprocess import call

def makeRecursive(tooldir, srcdirname, targetname):
    for root, dirs, files in walk(srcdirname): 
        newdir = join(targetname, basename(root))
        mkdir(newdir) 
        for f in filter((lambda f: f.endswith('.html')), files):
            (truncated, ext) = splitext(f)
            newfile = join(newdir, '.'.join([truncated, 'ids', 'html']))
            cssfilename = '.'.join([truncated, 'css'])
            call(['xsltproc', '--path', tooldir, '--stringparam', 'stylesheet', cssfilename, '-o', newfile, 'id-lines-overlay.xsl', join(root, f)])  

def main():
    parser = OptionParser(usage='%prog [options] tooldir srcdirname targetdirname')

    parser.add_option('-f', '--force', action='store_true', default=False, help='overwrite current directory, if exists')  

    options, args = parser.parse_args()

    # checking that script was called correctly
    if len(args) != 3: parser.error('wrong number of positional arguments')

    tooldir = args[0]
    srcdirname = args[1] 
    targetname = args[2]

    if not(isdir(tooldir)):
        print >>sys.stderr, 'Script directory %s does not exist.' % tooldir 
        sys.exit(1)

    if not(isdir(srcdirname)):
        print >>sys.stderr, "Directory %s does not exist." % srcdirname 
        sys.exit(1)

    if not(exists(targetname)) and not(options.force):
        print >>sys.stderr, 'Target %s does not exist.' % targetname 
        sys.exit(1)
    
    target = join(targetname, basename(srcdirname)) 

    if exists(target) and not(options.force):
        print >>sys.stderr, 'Target %s already exists.' % target
        print >>sys.stderr, 'Use --force to overwrite.' 
        sys.exit(1)
    
    if exists(target):
        if not(isdir(target)):
            remove(target)
        else:
            rmtree(target)
    makeRecursive(tooldir, srcdirname, targetname)

if __name__ == '__main__':
    main()

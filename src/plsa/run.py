#!/usr/bin/python
# -*- python -*-

import sys

from optparse import OptionParser
from os import mkdir, spawnvp, P_WAIT
from os.path import exists, join

def main():
    parser = OptionParser(usage='%prog [options] tooldir inputfile numaspects numbugaspects') 

    parser.add_option("-r", "--restarts", action="store", type="int", dest="numrestarts", default=1, metavar="NUMRESTARTS", help="number of restarts for the experiment; defaults to 1")
    parser.add_option("-i", "--maxiterations", action="store", type="int", dest="maxiterations", default=100, metavar="MAXITERATIONS", help="maximum number of iterations")
    parser.add_option("-d", "--minlikelihoodchange", action="store", type="float", dest="minlikelihoodchange", default=1.0, metavar="MINLIKELIHOODCHANGE", help="minimum likelihood change") 
    parser.add_option("-o", "--outputfilename", action="store", type="string", dest="outputfilename", default="results.mat", metavar="OUTPUTFILENAME", help="output file name") 
    parser.add_option("-l", "--logfilename", action="store", type="string", dest="logfilename", default="loglikelihood.txt", metavar="LOGFILENAME", help="file for logging the final value of the log of the likelihood")

    options, args = parser.parse_args()

    # checking that script was called correctly 
    if len(args) != 4: parser.error('wrong number of positional arguments')

    tooldir, inputfile, numaspects, numbugaspects = args

    params = '-nodisplay' 
    pathcall = "".join(['path(', "".join(['\'', tooldir, '\'']), ', path)'])
    quitcall = 'quit'

    for restart in range(int(options.numrestarts)):
        curdir = str(restart)
        outputfile = join(curdir, options.outputfilename)
        logfile = join(curdir, options.logfilename)
        if exists(outputfile):
            print 'skipping %s because it has already been made' % outputfile 
        else:
            if not(exists(curdir)):
                mkdir(curdir) 
            functionargs = ["".join(['\'', inputfile, '\''])]
            functionargs.append("".join(['\'', outputfile, '\'']))
            functionargs.append("".join(['\'', logfile, '\'']))
            functionargs.append(numaspects)
            functionargs.append(numbugaspects)
            functionargs.append('%u' % options.maxiterations)
            functionargs.append('%f' % options.minlikelihoodchange)
            functionargs = ", ".join(functionargs)
            functioncall = "".join(['runOnce(', functionargs, ')'])
            rstring = "; ".join([pathcall, functioncall, quitcall]) 
            spawnvp(P_WAIT, 'matlab', [params, '-r', rstring]) 

if __name__ == '__main__':
    main()

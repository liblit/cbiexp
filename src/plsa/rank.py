#!/s/std/bin/python
# -*- python -*-

from optparse import OptionParser
from os.path import join
from shutil import copytree
from subprocess import call

def main():
    parser = OptionParser(usage='%prog [options]') 

    parser.add_option("-r", "--restart", action="append", type="string", dest="restarts", default=[], metavar="RESTART", help="restarts in experiment to rank; may be specified multiple times")
    parser.add_option("-l", "--logfilename", action="store", type="string", dest="logfilename", default="loglikelihood.txt", metavar="LOGFILENAME", help="file for logging the final value of the log of the likelihood")

    options, args = parser.parse_args()

    # checking that script was called correctly 
    if len(args) != 0: parser.error('wrong number of positional arguments')

    def loglikelihood(restart):
        curdir = str(restart)
        rankfilename = join(curdir, options.logfilename)
        try:
            rankfile = open(rankfilename, 'r')
            ll = rankfile.next() 
            return float(ll.strip())
        except Exception:
            raise Exception('Could not get log likelihood for restart %s' % restart)

    loglikelihoods = map(lambda e : (loglikelihood(e), e), map(lambda r : r.strip('/'), options.restarts))
    loglikelihoods.sort()
    rankfilename = options.logfilename 
    rankfile = open(rankfilename, 'w')
    for p in loglikelihoods: 
        print >>rankfile, "%.0f %s" % (p[0], p[1])
    rankfile.close() 

    best = loglikelihoods[-1][1] 
    copytree(best, 'best')


########################################################################


if __name__ == '__main__':
    main()

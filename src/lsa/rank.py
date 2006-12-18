#!/s/std/bin/python
# -*- python -*-

from optparse import OptionParser
from os.path import join
from subprocess import call

def main():
    parser = OptionParser(usage='%prog numrestarts') 

    options, args = parser.parse_args()

    # checking that script was called correctly 
    if len(args) != 1: parser.error('wrong number of positional arguments')

    numrestarts = args[0]

    def loglikelihood(restart):
        curdir = str(restart)
        rankfilename = join(curdir, 'loglikelihood.txt')
        try:
            rankfile = open(rankfilename, 'r')
            ll = rankfile.next() 
            return float(ll.strip())
        except Exception:
            raise Exception('Could not get log likelihood for restart %u' % restart)

    loglikelihoods = map(lambda e : (loglikelihood(e), e), range(int(numrestarts)))
    loglikelihoods.sort()
    rankfilename = 'loglikelihoods.txt'
    rankfile = open(rankfilename, 'w')
    for p in loglikelihoods: 
        print >>rankfile, "%.0f %u" % (p[0], p[1])
    rankfile.close() 

    best = loglikelihoods[-1][1]
    call(['ln','-s',str(best),'best']) 

      
    

         
             


########################################################################


if __name__ == '__main__':
    main()

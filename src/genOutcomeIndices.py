#!/usr/bin/python
from itertools import count, izip
from optparse import OptionParser

def main():
    """read outcomes and generate f.runs + s.runs"""
    parser = OptionParser(usage='%prog outcomesfile ffile sfile')

    options, args = parser.parse_args()
    if len(args) != 3: parser.error('wrong number of positional arguments')

    ffile = open(args[1], 'w')
    sfile = open(args[2], 'w')

    def thefile(outcome):
        if outcome == 0:
            return sfile
        if outcome > 0:
            return ffile
        raise ValueError('unexpected value for outcome %s' % outcome)

    ofile = open(args[0], 'r')
    for (c, t) in izip(count(0), ofile):
        print >>thefile(int(t.strip())), c

    ofile.close()

    ffile.close()
    sfile.close()

if __name__ == "__main__":
    main()

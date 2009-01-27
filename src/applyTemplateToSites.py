#!/s/python-2.5/bin/python -O
from optparse import OptionParser
from Cheetah.Template import Template
import CheetahFilters

def readSites(filepath):
    """generator to read sites.txt and return a dictionary for each site"""
    ifile = open(filepath, 'r')
    ifile.next()
    ifile.next()
    for line in ifile:
        fields = line.strip().split('\t')
        yield {'filepath':fields[1], 'lineno':fields[2], 'functionname':fields[3], 'cfgnode':fields[4], 'schemetag':fields[5], 'description':fields[6:len(fields)] }
    ifile.close()

def main():
    """reads sites.txt and applies a template, storing the output in dest"""
    parser = OptionParser(usage='%prog sitesfile dest templatefile')

    parser.add_option('-f', '--filter', action='store', default=None,
        help='specify filter')

    options, args = parser.parse_args()
    if len(args) != 3: parser.error('wrong number of positional arguments')

    if options.filter is None:
        filter = options.filter
    else:
        filter = CheetahFilters.factory(options.filter)

    ofile =  open(args[1], 'w')
    print >>ofile, Template(file=args[2], searchList=[{'filepath':args[0], 'readSites' : readSites}], filter=filter)
    ofile.close()


if __name__ == "__main__":
    main()

#!/s/std/bin/python

import common

from itertools import imap
from os.path import dirname
import sys

def main():
    if len(sys.argv) != 1:
        assert len(sys.argv) == 2
        samplingRate = float(sys.argv[1])
    else:
        samplingRate = 1
        
    # prepare to read data and apply basic filtering
    rows = common.rawData()
    rows = ( row for row in rows if row['SamplingRate'] == samplingRate )
    rows = ( row for row in rows if row['Effort'] == 5 )

    # prepare data storage area
    data = dict((app, []) for app in common.apps)
    data['Overall'] = []

    # collect and index individual data points of interest
    for row in rows:
        app = row['Application']
        best = row['compl_cr']
        if best != None:
            assert best in [0, 1, 2]
            data[app].append(best)
            data['Overall'].append(best)

    # LaTeX fragment with table rows
    subdir = dirname(__file__)
    latex = file(subdir + '/top-pred.tex', 'w')
    for app in common.apps:
        protected = app.replace('_', '\\_')
        variants = len(data[app])
        print >>latex, '\\prog{%s} & %d' % (protected, variants),

        for best in [0, 1, 2]:
            filtered = sum(imap(lambda code: code == best, data[app]))
            print >>latex, '& \\countcell{%d}' % filtered,

        print >>latex, '\\\\'

    # summary row
    print >>latex, '\\addlinespace'
    app = 'Overall'
    variants = len(data[app])
    print >>latex, 'Overall & %d' % variants,
    for best in [0, 1, 2]:
        filtered = sum(imap(lambda code: code == best, data[app]))
        print >>latex, '& %d\\%%' % (100. * filtered / variants),
    print >>latex, '\\\\'


if __name__ == '__main__':
    main()

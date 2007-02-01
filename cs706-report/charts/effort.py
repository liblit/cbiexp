#!/s/std/bin/python

import common

from itertools import izip
from pychart import *

import sys


def main():
    [outfile, kind] = sys.argv[1:]

    # prepare to read data and apply basic filtering
    rows = common.rawData()
    rows = ( row for row in rows if row['SamplingRate'] == 1 )

    # collect and index individual data points of interest
    data = {}
    for row in rows:
        coord = (row['Application'], row['Effort'])
        value = row[kind + '_interesting']
        if value == None: value = 0
        data.setdefault(coord, []).append(value)

    # summarize by averaging values at each (app, effort) coordinate
    for coord, counts in data.iteritems():
        data[coord] = common.average(counts)

    # extract effort levels to serve as X axis category labels
    efforts = ( coord[1] for coord in data.iterkeys() )
    efforts = set(efforts)
    efforts = sorted(efforts)
    categories = [ (effort,) for effort in efforts ]

    # tick marks, in same order as common.apps
    # line styles rotate automatically
    ticks = [tick_mark.blackcircle3, tick_mark.blacksquare3, tick_mark.blacktri3, tick_mark.blackdtri3, tick_mark.plus3, tick_mark.x3]

    # create plot area
    theme.output_file = outfile
    common.setTheme()
    x_coord = category_coord.T(categories, 0)
    x_axis = axis.X(label='/ieffort', format='%d%%')
    y_coord = log_coord.T()
    y_axis = axis.Y(label='Number of interesting predicates', format=common.format_count)

    loc = {'conj': (100, 20), 'disj': (130, 7)}[kind]
    leg = legend.T(loc=loc)

    ar = area.T(x_axis=x_axis, x_coord=x_coord,
                y_axis=y_axis, y_coord=y_coord,
                legend=leg)

    # add one line plot for each application
    for (app, tick) in izip(common.apps, ticks):
        slice = [ (effort, data[app, effort]) for effort in efforts ]
        line = line_plot.T(label='/C/6' + app, data=slice, tick_mark=tick)
        ar.add_plot(line)

    # save the rendered result
    ar.draw()


if __name__ == '__main__':
    main()

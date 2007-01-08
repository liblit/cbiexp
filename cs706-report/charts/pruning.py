#!/usr/bin/python

from csv import DictReader
from pychart import *

import sys


def main(raw, out):
    # match fonts used in body of paper
    theme.default_font_family = 'Times'
    theme.reinitialize()

    # prepare to read data and apply basic filtering
    raw = file(raw)
    rows = DictReader(raw, delimiter=' ')
    rows = ( row for row in rows if row['SamplingRate'] == '1' )
    rows = ( row for row in rows if row['Effort'] == '5' )

    # collect and index individual data points of interest
    data = {}
    for row in rows:
        app = row['Application']
        total = int(row['Total'])
        pruned = int(row['Skipped'])
        computed = int(row['Computed'])
        interesting = int(row['Interesting'])
        print app, total, pruned + computed

    # summarize by averaging values at each (app, effort) coordinate
    for coord, counts in data.iteritems():
        data[coord] = float(sum(counts)) / len(counts)

    # extract effort levels to serve as X axis category labels
    efforts = ( coord[1] for coord in data.iterkeys() )
    efforts = set(efforts)
    efforts = sorted(efforts)
    categories = [ (effort,) for effort in efforts ]

    # create plot area
    x_coord = category_coord.T(categories, 0)
    x_axis = axis.X(label='/ieffort', format='%d%%')
    y_axis = axis.Y(label='Number of interesting predicates', format=format_y, tic_interval=10000)
    size = (110, 100)
    ar = area.T(x_axis=x_axis, y_axis=y_axis,
                x_coord=x_coord,
                y_grid_interval=5000,
                size=size)

    # applications and tick marks; line styles rotate automatically
    apps = [
        ('print_tokens2', tick_mark.blackcircle3),
        ('replace', tick_mark.blacksquare3),
        ('schedule', tick_mark.blacktri3),
        ('schedule2', tick_mark.blackdtri3),
        ('tcas', tick_mark.plus3),
        ('tot_info', tick_mark.x3),
        ]

    # add one line plot for each application
    for (app, tick) in apps:
        slice = [ (effort, data[app, effort]) for effort in efforts ]
        line = line_plot.T(label=app, data=slice, tick_mark=tick)
        ar.add_plot(line)

    # save rendered plot
    ar.draw(canvas.init(out))


if __name__ == '__main__':
    main(*sys.argv[1:])

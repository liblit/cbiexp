#!/s/std/bin/python

import common

from itertools import imap
from pychart import *


def complex(best):
    return best in set([1, 2])


def main():
    # prepare to read data and apply basic filtering
    rows = common.rawData()
    rows = ( row for row in rows if row['SamplingRate'] == 1 )
    rows = ( row for row in rows if row['Effort'] == 5 )

    # collect and index individual data points of interest
    data = {}
    for row in rows:
        app = row['Application']
        best = row['Complex']
        assert best in [0, 1]           # 2 == disjunction, never best in our experiments
        data.setdefault(app, []).append(best)

    # for each app, compute fraction having complex (non-0) best
    for app, counts in data.iteritems():
        data[app] = float(sum(imap(complex, counts))) / len(counts)

    # flatten into ordered data array for use by PyChart
    data = [ (app, data[app]) for app in common.apps ]

    # create plot area
    common.setTheme()
    x_coord = common.appsCoord()
    x_axis = common.appsAxisX()
    y_axis = axis.Y(label='Variants with Complex\nTop Predictor', format=common.format_percent)
    ar = area.T(x_axis=x_axis, x_coord=x_coord,
                y_axis=y_axis, y_range=(0, 1), y_grid_interval=0.1,
                legend=None)

    # add bars and save the rendered result
    bars = bar_plot.T(data=data, fill_style=fill_style.gray50)
    ar.add_plot(bars)
    ar.draw()


if __name__ == '__main__':
    main()

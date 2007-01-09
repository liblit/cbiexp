#!/s/std/bin/python

import common

from pychart import *


lastBarPlot = None


def addBars(label, series):
    global lastBarPlot
    data = list(series.iteritems())
    plot = bar_plot.T(label=label, data=data, stack_on=lastBarPlot)
    lastBarPlot = plot
    return plot


fates = ['Pruned by\nusefulness metric', 'Pruned by\noptimizations', 'Computed']


def main():
    # prepare to read data and apply basic filtering
    rows = common.rawData()
    rows = ( row for row in rows if row['SamplingRate'] == 1 )
    rows = ( row for row in rows if row['Effort'] == 5 )

    # collect and index individual data points of interest
    data = dict((fate, {}) for fate in fates)
    for row in rows:
        app = row['Application']
        total = float(row['Total'])

        computed = row['Computed'] / total
        optimized = row['Skipped'] / total
        metric = 1 - (computed + optimized)

        data['Computed'].setdefault(app, []).append(computed)
        data['Pruned by\noptimizations'].setdefault(app, []).append(optimized)
        data['Pruned by\nusefulness metric'].setdefault(app, []).append(metric)

    # summarize by averaging at each data point
    for fate, series in data.iteritems():
        for app, values in series.iteritems():
            series[app] = sum(values) / len(values)
        values = list(series.itervalues())
        series['Overall'] = sum(values) / len(values)

    # create plot area
    common.setTheme()
    x_coord = common.appsCoord(overall=True)
    x_axis = common.appsAxisX()
    y_axis = axis.Y(label='Fraction of complex predicates', format=common.format_percent)
    size = (110, 100)
    ar = area.T(x_axis=x_axis, x_coord=x_coord,
                y_axis=y_axis, y_range=(0, 1), y_grid_interval=0.1,
                size=size)

    # add stacked bars in reverse order so legend looks right
    plots = [ addBars(fate, data[fate]) for fate in fates ]
    plots.reverse()
    ar.add_plot(*plots)

    # save rendered plot
    ar.draw()


if __name__ == '__main__':
    main()

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


pruneEffort = 'Prune: {/ieffort} > 5%'
pruneBound = 'Prune: score upper bound too low'
computeDiscard = 'Compute exact score, but too low'
computeRetain = 'Compute exact score and retain'
fates = [computeRetain, computeDiscard, pruneBound, pruneEffort]


def main():
    # prepare to read data and apply basic filtering
    rows = common.rawData()
    rows = ( row for row in rows if row['SamplingRate'] == 1 )
    rows = ( row for row in rows if row['Effort'] == 5 )

    # collect and index individual data points of interest
    data = dict((fate, {}) for fate in fates)
    for row in rows:
        app = row['Application']

        compute = row['Computed']
        retain = row['Interesting']
        waste = compute - retain
        bound = row['Skipped']
        total = row['Total']
        effort = total - (compute + bound)
        assert retain + waste + bound + effort == total

        total = float(row['Total'])
        data[computeRetain].setdefault(app, []).append(retain / total)
        data[computeDiscard].setdefault(app, []).append(waste / total)
        data[pruneBound].setdefault(app, []).append(bound / total)
        data[pruneEffort].setdefault(app, []).append(effort / total)

    # summarize by averaging at each data point
    for fate in fates:
        series = data[fate]
        for app, values in series.iteritems():
            series[app] = sum(values) / len(values)
        values = list(series.itervalues())
        series['Overall'] = sum(values) / len(values)
        print 'Overall: %.0f%%: %s' % (series['Overall'] * 100, fate)

    # create plot area
    common.setTheme()
    x_coord = common.appsCoord(overall=True)
    x_axis = common.appsAxisX()
    y_axis = axis.Y(label='Fraction of complex predicates', format=common.format_percent)
    leg = legend.T(loc=(20, 100), shadow=(1, -1, fill_style.gray50))
    ar = area.T(x_axis=x_axis, x_coord=x_coord,
                y_axis=y_axis, y_range=(0, 1), y_grid_interval=0.1,
                legend=leg)

    # add stacked bars in reverse order so legend looks right
    plots = [ addBars(fate, data[fate]) for fate in fates ]
    ar.add_plot(*reversed(plots))

    # save rendered plot
    ar.draw()


if __name__ == '__main__':
    main()

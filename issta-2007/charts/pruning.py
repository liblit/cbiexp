#!/s/python-2.7.1/bin/python -O

import common

from itertools import count, izip
from pychart import *

import sys


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
    [outfile, kind] = sys.argv[1:]

    # prepare to read data and apply basic filtering
    rows = common.rawData()
    rows = ( row for row in rows if row['SamplingRate'] == 1 )
    rows = ( row for row in rows if row['Effort'] == 5 )

    # prepare master data table
    data = dict(((app, fate), [])
                for app in common.apps
                for fate in fates)

    # collect and index individual data points of interest
    for row in rows:
        app = row['Application']

        total = row[kind + '_total']
        if total != None:
            compute = row[kind + '_computed']
            retain = row[kind + '_interesting']
            waste = compute - retain
            bound = row[kind + '_ub_est_pruned']
            effort = row[kind + '_pdg_metric_pruned']
            assert retain + waste + bound + effort == total

            total = float(total)
            data[app, computeRetain].append(retain / total)
            data[app, computeDiscard].append(waste / total)
            data[app, pruneBound].append(bound / total)
            data[app, pruneEffort].append(effort / total)

    # summarize by averaging at each data point
    for coord, values in data.iteritems():
        data[coord] = common.average(values)

    # create plot area
    theme.output_file = outfile
    common.setTheme()
    x_coord = common.appsCoord(overall=True)
    x_axis = common.appsAxisX()
    y_axis = axis.Y(label='Fraction of complex predicates', format=common.format_percent)
    leg = legend.T(loc=(23, 100), shadow=(1, -1, fill_style.gray50))
    ar = area.T(x_axis=x_axis, x_coord=x_coord,
                y_axis=y_axis, y_range=(0, 1), y_grid_interval=0.1,
                legend=leg)

    # add stacked bars in reverse order so legend looks right

    def plots():
        prior = None
        for fate in fates:
            series = [ (app, data[app, fate]) for app in common.apps ]
            overall = common.average([ point[1] for point in series ])
            series.append(('Overall', overall))
            bars = bar_plot.T(data=series, stack_on=prior, label=fate)
            prior = bars
            yield bars

    bars = reversed(list(plots()))
    ar.add_plot(*bars)

    # save rendered plot
    ar.draw()


if __name__ == '__main__':
    main()

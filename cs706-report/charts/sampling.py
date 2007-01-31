#!/s/std/bin/python

import common

from itertools import imap
from os.path import dirname
from pychart import *

import locale


def format_x(density):
    return '/6{}1//%s' % density


def main():
    subdir = dirname(__file__)

    # prepare to read data and apply basic filtering
    rows = common.rawData()
    rows = ( row for row in rows if row['Effort'] == 5 )

    # prepare master data table
    densities = [1, 1.01, 2, 5, 10, 100, 1000]
    kinds = ['Conjunctions', 'Simple', 'Disjunctions']
    data = dict(((app, density, kind), [])
                for app in common.apps
                for density in densities
                for kind in kinds)

    # collect and index individual data points of interest
    for row in rows:
        app = row['Application']
        density = row['SamplingRate']

        def record(kind, column):
            count = row[column]
            if count != None:
                data[app, density, kind].append(count)

        record('Conjunctions', 'conj_interesting')
        record('Disjunctions', 'disj_interesting')
        record('Simple', 'simple_preds')

    # summarize by averaging values at each (app, density, kind) coordinate
    for coord, counts in data.iteritems():
        data[coord] = common.average(counts)
        if data[coord] == 0:
            data[coord] = None

    # create common plot area elements
    common.setTheme(size=(120, 93))
    densities = [1, 1.01, 2, 5, 10, 100, 1000]
    categories = [ (label,) for label in densities ]
    x_coord = category_coord.T(categories, 0)
    x_axis = axis.X(label='Sampling Rate', format=format_x)
    y_coord = log_coord.T()
    y_axis = axis.Y(label='Number of Predicates', format=common.format_count)

    # create single-application plot
    theme.reinitialize()
    leg = legend.T(loc=(115, 10))
    ar = area.T(x_axis=x_axis, x_coord=x_coord,
                y_axis=y_axis, y_coord=y_coord,
                legend=leg)

    # one data series (line) per category of counted predicates
    app = 'print_tokens2'
    for series in kinds:
        counts = [ (density, data[app, density, series]) for density in densities ]
        ar.add_plot(line_plot.T(label=series, data=counts))

    # render plots to disk
    ar.draw(canvas.init('%s/sampling-%s.pdf' % (subdir, app)))
    ar.draw(canvas.init('%s/sampling-%s.eps' % (subdir, app)))

    # LaTeX fragment with table rows
    latex = file(subdir + '/sampling.tex', 'w')
    for app in common.apps:
        protected = app.replace('_', '\\_')
        print >>latex, '\\prog{%s}' % protected,
        for series in ['Simple', 'Conjunctions', 'Disjunctions']:
            for density in [1, 100, 1000]:
                count = data[app, density, series]
                if count == None:
                    cell = '-'
                else:
                    cell = locale.format('%.0f', count, grouping=True)
                print >>latex, '&', cell,
        print >>latex, '\\\\'


if __name__ == '__main__':
    main()

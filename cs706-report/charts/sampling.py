#!/s/std/bin/python

import common

from itertools import imap
from os.path import dirname
from pychart import *

import locale


def format_x(density):
    return '/6{}1//%s' % density


def format_y(count):
    return locale.format('/6{}%d', count, grouping=True)


def main():
    locale.setlocale(locale.LC_ALL, '')
    subdir = dirname(__file__)

    # prepare to read data and apply basic filtering
    rows = common.rawData()
    rows = ( row for row in rows if row['Effort'] == 5 )

    # collect and index individual data points of interest
    data = {}
    for row in rows:
        app = row['Application']
        density = row['SamplingRate']

        def record(kind, column):
            count = row[column]
            if count == None: count = 0
            data.setdefault((app, density, kind), []).append(count)

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
    y_axis = axis.Y(label='Number of Predicates', format=format_y)

    # create per-application plots
    for app in common.apps:
        theme.reinitialize()

        leg = legend.T()
        ar = area.T(x_axis=x_axis, x_coord=x_coord,
                    y_axis=y_axis, y_coord=y_coord,
                    legend=leg)

        # one data series (line) per category of counted predicates
        for series in ['Conjunctions', 'Disjunctions', 'Simple']:
            counts = [ (density, data[app, density, series]) for density in densities ]
            ar.add_plot(line_plot.T(label=series, data=counts))

        ar.draw(canvas.init('%s/sampling-%s.pdf' % (subdir, app)))
        ar.draw(canvas.init('%s/sampling-%s.eps' % (subdir, app)))

    # helper LaTeX fragment to pull in all plots
    latex = file(subdir + '/sampling.tex', 'w')
    for app in common.apps:
        protected = app.replace('_', '\\_')
        print >>latex, '\\plot{%s}{%s}' % (protected, app)

if __name__ == '__main__':
    main()

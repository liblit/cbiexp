import sys
sys.path.insert(1, '/unsup/pychart/lib/python')

from csv import DictReader
from itertools import chain
from os.path import dirname
from pychart import *


def rawData():
    filename = dirname(__file__) + '/../data/data.txt'
    stream = file(filename)
    reader = DictReader(stream, delimiter=' ')

    for row in reader:
        def fix(coercion, key):
            if row[key] == '**':
                row[key] = None
            else:
                row[key] = coercion(row[key])

        fix(int, 'Version')
        fix(float, 'SamplingRate')
        fix(int, 'Effort')

        fix(int, 'Total')
        fix(int, 'Skipped')
        fix(int, 'Computed')
        fix(int, 'Interesting')

        fix(float, 'Top')
        fix(float, 'Bottom')
        fix(float, 'PredTop')
        fix(int, 'Complex_Perf')
        fix(int, 'Simple_Perf')
        fix(int, 'Preds')

        fix(int, 'Complex')
        fix(bool, 'PerfComplex')
        fix(int, 'Base')
        fix(bool, 'PerfBase')

        yield row


def setTheme():
    chart_object.set_defaults(bar_plot.T, width=10)
    chart_object.set_defaults(area.T, size=(180, 140))
    theme.default_font_family = 'Times'
    [outfile] = sys.argv[1:]
    theme.output_file = outfile
    theme.reinitialize()


apps = ['print_tokens2', 'replace', 'schedule', 'schedule2', 'tcas', 'tot_info']


def appsCoord(overall=False):
    labels = apps
    if overall: labels = chain(labels, ['Overall'])
    categories = [ (label,) for label in labels ]
    return category_coord.T(categories, 0)


def __format_app(app):
    if app == 'print_tokens2': app = 'print_\ntokens2'
    if app != 'Overall': app = '/C' + app
    return '/a90/6' + app


def appsAxisX():
    return axis.X(label='Application', format=__format_app)


def format_percent(value):
    return '/6{}%.0f%%' % round(value * 100)

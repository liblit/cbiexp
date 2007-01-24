import sys
sys.path.insert(1, '/unsup/pychart/lib/python')

from csv import DictReader
from itertools import chain
from os.path import dirname
from pychart import *

import locale


apps = ['print_tokens', 'print_tokens2', 'replace', 'schedule', 'schedule2', 'tcas', 'tot_info']


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

        assert row['Application'] in apps

        fix(int, 'Version')
        fix(float, 'SamplingRate')
        fix(int, 'Effort')

        fix(int, 'simple_preds')
        fix(int, 'time')
        
        fix(int, 'conj_total')
        fix(int, 'conj_pdg_metric_pruned')
        fix(int, 'conj_ub_est_pruned')
        fix(int, 'conj_computed')
        fix(int, 'conj_interesting')

        fix(int, 'disj_total')
        fix(int, 'disj_pdg_metric_pruned')
        fix(int, 'disj_ub_est_pruned')
        fix(int, 'disj_computed')
        fix(int, 'disj_interesting')
        
        fix(float, 'top_compl')
        fix(float, 'hundredth_compl')
        
        fix(int, 'num_perf_compl')
        fix(int, 'num_perf_simple')
        
        fix(float, 'top_simple')
        
        fix(int, 'compl_cr')
        fix(int, 'compl_cr_is_top_perf')
        
        fix(int, 'simple_cr')
        fix(int, 'simple_cr_is_top_perf')

        yield row


def setTheme(size=(180, 140)):
    locale.setlocale(locale.LC_ALL, '')
    chart_object.set_defaults(bar_plot.T, width=10)
    chart_object.set_defaults(area.T, size=size)
    theme.default_font_family = 'Times'
    theme.reinitialize()


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


def format_count(count):
    return '/6{}' + locale.format('%d', count, grouping=True)


def average(values):
    return float(sum(values)) / len(values)

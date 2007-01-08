from csv import DictReader
from os.path import dirname
from pychart import *

import sys


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
    theme.default_font_family = 'Times'
    [outfile] = sys.argv[1:]
    theme.output_file = outfile
    theme.reinitialize()


apps = ['print_tokens2', 'replace', 'schedule', 'schedule2', 'tcas', 'tot_info']

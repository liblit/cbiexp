#!/s/std/bin/python

import common

from itertools import count, izip

def average_time(fname):
    data = file(fname).readlines()[1:]
    data = [line.split(' ')[-1] for line in data]
    data = [int(line) for line in data]    
    return common.average(data)
    
def main():
    # prepare to read data and apply basic filtering
    rows = common.rawData()
    rows = ( row for row in rows if row['SamplingRate'] == 1 )
    rows = ( row for row in rows if row['Effort'] == 5 )

    percents = []
    time_aggressive = []
    time_only_metrics = []
    for row in rows:
        if row['conj_total'] == None:
            continue
        pruned = row['compl_cr_pruned_conj'] + row['compl_cr_pruned_disj']
        total = row['conj_total'] + row['disj_total']
        percents.append(float(pruned) / total)
        
        time_aggressive.append(row['time_compl_cr'])
        time_only_metrics.append(row['time'])
        
    print 'Aggressive pruning efficiency: %lf of total' % common.average(percents)
    
    print '\nTimes:'
    print 'When everything is computed (no pruning, no metrics): %d seconds' % average_time('data/times-compute-all.txt')
    print 'Only metrics are applied: %d seconds' % common.average(time_only_metrics)
    print 'Moderate pruning is applied: %d seconds' % average_time('data/times-moderate-pruning.txt')    
    print 'Aggressive pruning is applied: %d seconds' % common.average(time_aggressive)


if __name__ == '__main__':
    main()

#!/s/std/bin/python
# -*- python -*-

import csv

def main():
    """Transforms run-bugs.txt format to sparse matrix format""" 
    bugfile = open('run-bugs.txt', 'r')
    oraclefile = open('oracle.sparse', 'w')
    writer = csv.writer(oraclefile, delimiter='\t',lineterminator='\n')
    reader = csv.reader(bugfile, delimiter='\t') 
    for l in reader: 
        col = int(l[0]) + 1
        rows = l[1:len(l)]
        for r in rows:
            writer.writerow([r, col, 1])
    return;        


      
    

         
             


########################################################################


if __name__ == '__main__':
    main()

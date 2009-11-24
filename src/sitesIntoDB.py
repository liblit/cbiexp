#!/s/python-2.5/bin/python -O
# -*- python -*-

from __future__ import with_statement

import re
import sqlite3
import sys

from itertools import count

from initializeSchema import EnumerationTables

# Assuming all tables have already been created by initializeSchema.py
###############################################################################

schemeTables = {
        'atoms': 'AtomsSiteDescriptors',
        'bounds': 'BoundsSiteDescriptors',
        'branches': 'BranchesSiteDescriptors',
        'float-kinds': 'FloatKindsSiteDescriptors',
        'function-entries': None,
        'g-object-unref': 'GObjectUnrefSiteDescriptors',
        'returns': 'ReturnsSiteDescriptors',
        'scalar-pairs': 'ScalarPairsSiteDescriptors'
        }

numTableColumns = {
        'atoms': 3,
        'bounds': 4,
        'branches': 2,
        'float-kinds': 4,
        'function-entries': None,
        'g-object-unref': 2,
        'returns': 2,
        'scalar-pairs': 6
        }

###############################################################################

def readSites(filepath):
    keys = ('SiteID',
            'SchemeID',
            'FileName',
            'Line',
            'FunctionIdentifier',
            'CFGNode',
            'Descriptors')
    matcher = re.compile('<sites unit="(?P<sig>[0-9a-f]{32})" scheme="(?P<sch>[a-zA-Z\-]*)">')

    SiteID = count()
    SchemeID = None
    with file(filepath, 'r') as ifile:
        for lineNum, line in enumerate(ifile):
            match = matcher.match(line)
            if match:
                Scheme = match.group('sch')
                SchemeID = filter(lambda x: x[1]==Scheme, 
                                    EnumerationTables['Schemes'])[0][0]
            elif line.startswith('</sites>'):
                # closing sites tag
                SchemeID = None
            else:
                vals = [SiteID.next(), SchemeID] + line.strip().split(None, 4)
                yield dict(zip(keys, vals))


def process(sitesFile, dbname):
    con = sqlite3.connect(dbname)
    cursor = con.cursor()

    for site in readSites(sitesFile):
        cursor.execute("INSERT INTO Sites VALUES(\
                            :SiteID, :FileName, :Line, :FunctionIdentifier,\
                            :CFGNode, :SchemeID);",
                        site)

        scheme = filter(lambda x: x[0] == site['SchemeID'],
                            EnumerationTables['Schemes'])[0][1]
        if schemeTables[scheme]:
            table = schemeTables[scheme]
            numCols = numTableColumns[scheme]
            entries = ', '.join('?' * numCols)

            query = "INSERT INTO %s VALUES (%s);" % (table, entries)
            cursor.execute(query, [site['SiteID']] + site['Descriptors'].split('\t'))

    con.commit()

def main():
    if len(sys.argv) != 3:
        print 'Usage: %s <sites-file> <sqlite-database>' % sys.argv[0]
    process(*sys.argv[1:])

if __name__ == '__main__': main()

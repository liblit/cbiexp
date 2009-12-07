#!/s/python-2.5/bin/python -O
# -*- python -*-

from __future__ import with_statement

import optparse
import re
import sqlite3
import sys

from itertools import count

from initializeSchema import EnumerationTables

###############################################################################

schemeTables = {
        'atoms': 'AtomsSiteDescriptors',
        'bounds': 'BoundsSiteDescriptors',
        'branches': 'BranchesSiteDescriptors',
        'compare-swap': 'CompareSwapSiteDescriptors',
        'float-kinds': 'FloatKindsSiteDescriptors',
        'function-entries': None,
        'g-object-unref': 'GObjectUnrefSiteDescriptors',
        'returns': 'ReturnsSiteDescriptors',
        'scalar-pairs': 'ScalarPairsSiteDescriptors'
        }

def getEnumEvaluator(enumName):
    enum = dict((t[1], t[0]) for t in EnumerationTables[enumName])
    def evaluator(key):
        return enum[key]
    return evaluator

convertors = {
        'AccessType': getEnumEvaluator('AccessTypes'),
        'AssignmentType': getEnumEvaluator('AssignmentTypes'),
        'VariableType': getEnumEvaluator('VariableTypes'),
        'Text': lambda x: x
        }

SchemeDescriptions = {
        'atoms': ('Text', 'AccessType',),
        'bounds': ('Text', 'VariableType', 'AccessType',),
        'branches': ('Text',),
        'compare-swap': ('Text',),
        'float-kinds': ('Text', 'VariableType', 'AccessType',),
        'function-entries': None,
        'g-object-unref': ('Text',),
        'returns': ('Text',),
        'scalar-pairs': ('Text', 'VariableType', 'AssignmentType',\
                         'Text', 'VariableType',),
        }

###############################################################################

def readSites(filepath):
    keys = ('SiteID',
            'SchemeName',
            'FileName',
            'Line',
            'FunctionIdentifier',
            'CFGNode',
            'Descriptors')
    matcher = re.compile('<sites unit="(?P<sig>[0-9a-f]{32})" scheme="(?P<sch>[a-zA-Z\-]*)">')

    SiteID = count()
    SchemeName = None
    with file(filepath, 'r') as ifile:
        for line in ifile:
            match = matcher.match(line)
            if match:
                SchemeName = match.group('sch')
            elif line.startswith('</sites>'):
                SchemeName = None
            else:
                vals = [SiteID.next(), SchemeName] + line.strip().split(None, 4)
                yield dict(zip(keys, vals))


def writeSitesIntoDB(conn, sitesTxt, version):
    cursor = conn.cursor()

    schemeEnum = dict((t[1], t[0]) for t in EnumerationTables['Schemes'])
    for site in readSites(sitesTxt):
        site['SchemeID'] = schemeEnum[site['SchemeName']]

        cursor.execute("INSERT INTO Sites VALUES(\
                            :SiteID, :FileName, :Line, :FunctionIdentifier,\
                            :CFGNode, :SchemeID);",
                        site)

        scheme = site['SchemeName']
        if schemeTables[scheme]:
            table = schemeTables[scheme]

            funcs = map(convertors.get, SchemeDescriptions[scheme])
            args = site['Descriptors'].split('\t')
            values = [site['SiteID']] + map(lambda x, y: x(y), funcs, args)

            entries = ', '.join('?' * len(values))
            query = "INSERT INTO %s VALUES (%s);" % (table, entries)
            cursor.execute(query, values)

    conn.commit()

def main():
    parser = optparse.OptionParser(usage='%prog <database> <sites-file>')
    parser.add_option('-v', '--version', action='store', default=1,
                      help = 'version of schema to implement')

    option, args = parser.parse_args(sys.argv[1:])
    if len(args) != 2:
        parser.error('wrong number of positional arguments')
    if options.version != 1:
        parser.error('CBI database schema version ' + options.version +
                     ' is currently not supported')

    cbi_db, sitesTxt = args

    conn = sqlite3.connect(cbi_db)
    writeSitesIntoDB(conn, sitesTxt, options.version)
    conn.close()

if __name__ == '__main__': main()

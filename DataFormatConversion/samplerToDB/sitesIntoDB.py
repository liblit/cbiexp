#!/s/python-2.7.1/bin/python -O
# -*- python -*-

import optparse
import re
import sqlite3
import sys

from itertools import count, chain, imap, repeat

from DBConstants import EnumerationTables

###############################################################################

schemeTables = {
        'atoms': 'AtomsSiteDescriptors',
        'atoms-rw': 'AtomsRWSiteDescriptors',
        'bounds': 'BoundsSiteDescriptors',
        'branches': 'BranchesSiteDescriptors',
        'compare-swap': 'CompareSwapSiteDescriptors',
        'coverage': 'CoverageSiteDescriptors',
        'data': 'DataSiteDescriptors',
        'float-kinds': 'FloatKindsSiteDescriptors',
        'function-entries': None,
        'fun-reentries': None,
        'g-object-unref': 'GObjectUnrefSiteDescriptors',
        'returns': 'ReturnsSiteDescriptors',
        'scalar-pairs': 'ScalarPairsSiteDescriptors',
        'yields': 'YieldsSiteDescriptors',
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
        'atoms-rw': ('Text', 'AccessType',),
        'bounds': ('Text', 'VariableType', 'AccessType',),
        'branches': ('Text',),
        'compare-swap': ('Text',),
        'coverage': ('Text',),
        'data': ('Text', 'AccessType',),
        'float-kinds': ('Text', 'VariableType', 'AssignmentType',),
        'function-entries': None,
        'fun-reentries': None,
        'g-object-unref': ('Text',),
        'returns': ('Text',),
        'scalar-pairs': ('Text', 'VariableType', 'AssignmentType',\
                         'Text', 'VariableType',),
        'yields': ('Text',),
        }

###############################################################################

def readSitesFile(filepath, fileNameModifier):
    keys = ('FileName',
            'Line',
            'FunctionIdentifier',
            'CFGNode',
            'Descriptors')

    matcher = re.compile('<sites unit="(?P<Signature>[0-9a-f]{32})" scheme="(?P<SchemeName>[a-zA-Z\-]*)">')

    with file(filepath, 'r') as ifile:
        for line in ifile:
            match = matcher.match(line)
            if match:
                yield match.groupdict()
            elif line.startswith('</sites>'):
                yield None
            else:
                vals = line.strip().split(None, 4)
                if fileNameModifier:
                    vals[0] = fileNameModifier(vals[0])
                yield dict(zip(keys, vals))


def writeSitesIntoDB(conn, sitesFiles, fileNameModifier=None, wantedSchemes=None, version=1):
    """ Read units and sites from <sitesFiles> and insert into
        sqlite3 database connection <conn>.  Currently only CBI database 
        schema version 1 is supported.

        Optionally, you can also provide a <fileNameModifier> function to 
        modify the filenames (corresponding to the sites) being stored eg. 
        to strip the build/compilation directory path from the filename
    """

    if version != 1:
        raise ValueError('Version %s of the database schema is unsupported' %
                         str(version))

    cursor = conn.cursor()
    schemeNameToID = getEnumEvaluator('Schemes')
    if wantedSchemes is None:
        wantedSchemes = tuple(t[1] for t in EnumerationTables['Schemes'])

    SiteID = count()
    UnitID = count()

    curUnit, curScheme = None, None
    isFilteredScheme = False

    for line in chain(*imap(readSitesFile, sitesFiles, repeat(fileNameModifier))):
        if line is None:
            # end of unit
            curUnit, curScheme = None, None
            isFilteredScheme = False
            continue

        elif line.has_key('Signature'):
            # insert a new unit
            if curUnit is not None:
                raise ValueError('</sites> not found before %s' % line)

            if line['SchemeName'] not in wantedSchemes:
                isFilteredScheme = True
                continue

            curUnit = UnitID.next()
            curScheme = line['SchemeName']

            line['UnitID'] = curUnit
            line['SchemeID'] = schemeNameToID(curScheme)
            cursor.execute("INSERT INTO Units VALUES(\
                                :UnitID, :Signature, :SchemeID);",
                           line)
            continue

        elif isFilteredScheme:
            continue

        # else insert a site
        site = line
        site['SiteID'] = SiteID.next()
        site['UnitID'] = curUnit

        cursor.execute("INSERT INTO Sites VALUES(\
                            :SiteID, :FileName, :Line, :FunctionIdentifier,\
                            :CFGNode, :UnitID);",
                        site)

        if schemeTables[curScheme]:
            table = schemeTables[curScheme]

            funcs = map(convertors.get, SchemeDescriptions[curScheme])
            args = site['Descriptors'].split('\t')
            values = [site['SiteID']] + map(lambda x, y: x(y), funcs, args)

            entries = ', '.join('?' * len(values))
            query = "INSERT INTO %s VALUES (%s);" % (table, entries)
            cursor.execute(query, values)
    conn.commit()


def main():
    """ Insert units and sites into appropriate tables"""

    parser = optparse.OptionParser(usage='%prog <database> <sites-files>')
    parser.add_option('-s', '--scheme', action='append', dest='schemes',
                      choices=[t[1] for t in EnumerationTables['Schemes']],
                      help = 'add sites with scheme SCHEME.  All schemes\
                              will be processed if this flag is omitted')
    parser.add_option('-v', '--version', action='store', default=1, type='int',
                      help = 'version of schema to implement')

    options, args = parser.parse_args(sys.argv[1:])
    if len(args) < 2:
        parser.error('wrong number of positional arguments')
    if options.version != 1:
        parser.error('CBI database schema version ' + options.version +
                     ' is currently not supported')

    cbi_db = args[0]
    sitesFiles = args[1:]

    conn = sqlite3.connect(cbi_db)
    writeSitesIntoDB(conn, sitesFiles, options.version, options.schemes)
    conn.close()

if __name__ == '__main__': main()

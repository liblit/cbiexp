#!/s/python-2.7.1/bin/python

import CheetahFilters
import sqlite3

from itertools import chain, izip
from optparse import OptionParser
from Cheetah.Template import Template

schemeSpecificDescriptions = (
        # (name-of-table, extra columns added to static-site-info.cc)
        ('AtomsSiteDescriptors', ('Variable',)),
        ('AtomsRWSiteDescriptors', ('Variable',)),
        ('BoundsSiteDescriptors', ('Variable',)),
        ('BranchesSiteDescriptors', ('Predicate',)),
        ('CompareSwapSiteDescriptors', ('Variable',)),
        ('CoverageSiteDescriptors', ('Statement',)),
        ('DataSiteDescriptors', ('Variable',)),
        ('FloatKindsSiteDescriptors', ('Variable',)),
        ('GObjectUnrefSiteDescriptors', ('Object',)),
        ('ReturnsSiteDescriptors', ('Callee',)),
        ('ScalarPairsSiteDescriptors', ('LHSVariable', 'RHSVariable')),
        ('YieldsSiteDescriptors', ('Instruction',)),
        )


def getColumns(conn, table, colNames):
    """get columnames in <colNames> from <table>"""

    query = 'SELECT %s FROM %s' % (', '.join(colNames), table)
    cursor = conn.cursor()
    cursor.execute(query)
    descriptions = dict((t[0], t[1:]) for t in cursor)
    cursor.close()

    return descriptions


def readSites(cbi_db):
    """read sites from table 'Sites', fills in scheme-specific
       descriptions and 'yields' them to the Cheetah template
    """

    conn = sqlite3.connect(cbi_db)
    conn.text_factory = str

    auxDescriptions = dict()
    for table, colNames in schemeSpecificDescriptions:
        colNames = ('SiteID',) + colNames
        auxDescriptions.update(getColumns(conn, table, colNames))

    query = 'SELECT \
                SiteID, FileName, Line, FunctionIdentifier, CFGNode, SchemeCode \
            FROM \
                Sites JOIN Units JOIN Schemes ON\
                    Sites.UnitID=Units.UnitID AND\
                    Units.SchemeID=Schemes.SchemeID\
            ORDER BY Sites.SiteID'

    cursor = conn.cursor().execute(query)
    keys = [t[0] for t in cursor.description] + ['Descriptions']

    for row in cursor:
        auxInfo = auxDescriptions[row[0]]
        yield dict(izip(keys, chain(row, [auxInfo])))

    cursor.close()
    conn.close()


def genStaticSiteInfo(cbi_db, template, outFile, filter):
    if filter is not None:
        filter = CheetahFilters.factory(filter)

    template = Template(file=template, searchList=[{'filepath':cbi_db, 'readSites' : readSites}], filter=filter)
    with file(outFile, 'w') as ofile:
        print >>ofile, template


def main():
    """applies <template> to sites read from <database> and stores the
       output in <output-file>
    """

    parser = OptionParser(usage='%prog <database> <template> <output-file>')
    parser.add_option('-f', '--filter', action='store', default=None,
        help='specify filter')

    options, args = parser.parse_args()
    if len(args) != 3: parser.error('wrong number of positional arguments')

    genStaticSiteInfo(filter=options.filter, *args)

if __name__ == "__main__":
    main()

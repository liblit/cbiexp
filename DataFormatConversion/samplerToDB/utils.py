from collections import defaultdict

from DBConstants import EnumerationTables, ValueRecordingSchemes

def getSiteIDToSchemeMapping(conn):
    """ Returns a map from SiteIDs to SchemeIDs by querying
        database connection 'conn'.
    """

    cursor = conn.cursor()
    cursor.execute('SELECT SiteID, SchemeID FROM \
                        Sites JOIN Units ON Units.UnitID=Sites.UnitID')
    mapping = dict(cursor.fetchall())
    cursor.close()

    return mapping


def getSchemeIDToFieldMapping():
    """ Returns a map from SchemeID to a list of FieldIDs."""

    R = defaultdict(list)
    for t in EnumerationTables['Fields']:
        R[t[1]].append(t[0])

    return R


def getRangeForField(fieldID, store={}):
    """ Returns a list representing the range of FieldIDs for given FieldID.

        Args:
            fieldID: The FieldID whose range we want to determine
            store: A dictionary used to cache the results of previous calls
                    to the method. No value should be passed to it during a
                    call to the method.

        Returns:
            A list representing the range of FieldIDs for the scheme that the 
            fieldID corresponds to.
            If invalid fieldID is provided the return value is None
    """

    start = None
    end = None
    scheme = None
    foundScheme = False

    if fieldID in store:
        return range(*store[fieldID])
    else:
        for t in EnumerationTables['Fields']:
            if (scheme != t[1]):
                if not foundScheme:
                    start, scheme = t[0:2]
                    end = t[0]
                else:
                    break
            else:
                end = t[0]
            if end == fieldID:
                foundScheme = True
    
        if foundScheme:
            store[fieldID] = start, end+1
            return range(start, end+1)
        else:
            return None     


def getSchemeIDToTableMapping():
    """ Returns a map from instrumentation schemes to the
        table that holds its observed values.
    """

    R = {}
    for t in  EnumerationTables['Schemes']:
        if t[1] in ValueRecordingSchemes:
            R[t[0]] = 'SampleValues'
        else:
            R[t[0]] = 'SampleCounts'

    return R


class InsertQueryConstructor(object):
    """ Factory class that constructs SQL queries to insert
        samples into appropriate tables
    """

    def __init__(self):
        self.SchemeIDToFields = getSchemeIDToFieldMapping()
        self.SchemeIDToTable = getSchemeIDToTableMapping()
        self.Cols = ('Count', 'Phase', 'RunID', 'Thread', 'SiteID', 'FieldID')

    def generateInsertionScenario(self, schemeID):
        table = self.SchemeIDToTable[schemeID]
        cols = ', '.join(self.Cols)
        entries = ', '.join('?' * len(self.Cols))

        ignoreZeros = (self.SchemeIDToTable[schemeID] == 'SampleCounts')
        fields = self.SchemeIDToFields[schemeID]
        query = 'INSERT OR IGNORE INTO %s (%s) VALUES (%s);' % (table, cols, entries)
        return query, fields, ignoreZeros

    # This function is needed for the case of shared libraries where we may 
    # have multiple sample reports being generated for a single unit. 
    def generateUpdateQuery(self, schemeID):
        table = self.SchemeIDToTable[schemeID]
        return 'UPDATE %s SET Count = Count + ? WHERE Phase=? AND RunID=? AND Thread=? AND SiteID=? AND FieldID=?;' % (table, )


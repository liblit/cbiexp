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


def getSchemeIDToTableMapping():
    """ Reutns a map from instrumentation schemes to the
        table that holds its observed values.
    """

    R = {}
    for t in  EnumerationTables['Schemes']:
        if t[1] in ValueRecordingSchemes:
            R[t[0]] = 'SampleValues'
        else:
            R[t[0]] = 'SampleCounts'

    return R


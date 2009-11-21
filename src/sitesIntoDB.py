#!/s/python-2.5/bin/python -O
# -*- python -*-

from __future__ import with_statement

import re
import sqlite3
import sys

from itertools import imap

###############################################################################
# Table creation queries
###############################################################################

CreateQueries = """
CREATE TABLE AccessTypes(AccessType TEXT PRIMARY KEY NOT NULL);
INSERT INTO AccessTypes VALUES('read');
INSERT INTO AccessTypes VALUES('write');

CREATE TABLE AssignmentTypes(AssignmentType TEXT PRIMARY KEY NOT NULL);
INSERT INTO AssignmentTypes VALUES('direct');
INSERT INTO AssignmentTypes VALUES('field');
INSERT INTO AssignmentTypes VALUES('index');

CREATE TABLE VariableTypes(VariableType TEXT PRIMARY KEY NOT NULL);
INSERT INTO VariableTypes VALUES('local');
INSERT INTO VariableTypes VALUES('global');
INSERT INTO VariableTypes VALUES('mem');

CREATE TABLE Sites(
    ID INTEGER PRIMARY KEY NOT NULL,
    FileName TEXT NOT NULL,
    Line INT NOT NULL,
    FunctionIdentifier TEXT NOT NULL,
    CFGNode INT NOT NULL,
    Scheme_ID TEXT NOT NULL);

CREATE TABLE AtomsSiteDescriptors(
    ID INTEGER PRIMARY KEY REFERENCES Sites_pk,
    Variable TEXT NOT NULL,
    AccessType TEXT REFERENCES AccessTypes_pk);

CREATE TABLE BoundsSiteDescriptors(
    ID INTEGER PRIMARY KEY REFERENCES Sites_pk,
    Variable TEXT NOT NULL,
    VariableType TEXT REFERENCES VariableTypes_pk,
    AccessType TEXT REFERENCES AccessTypes_pk);

CREATE TABLE BranchesSiteDescriptors(
    ID INTEGER PRIMARY KEY REFERENCES Sites_pk,
    Predicate TEXT NOT NULL);

CREATE TABLE FloatKindsSiteDescriptors(
    ID INTEGER PRIMARY KEY REFERENCES Sites_pk,
    Variable TEXT NOT NULL,
    VariableType TEXT REFERENCES VariableTypes_pk,
    AccessType TEXT REFERENCES AccessTypes_pk);

CREATE TABLE GObjectUnrefSiteDescriptors(
    ID INTEGER PRIMARY KEY REFERENCES Sites(ID),
    Object TEXT NOT NULL);

CREATE TABLE ReturnsSiteDescriptors(
    ID INTEGER PRIMARY KEY REFERENCES Sites(ID),
    Callee TEXT NOT NULL);

CREATE TABLE ScalarPairsSiteDescriptors(
    ID INTEGER PRIMARY KEY REFERENCES Sites(ID),
    LHSVariable TEXT NOT NULL,
    LHSVariableType TEXT REFERENCES VariableTypes_pk,
    LHSAssignmentType TEXT REFERENCES AssignmentTypes_pk,
    RHSVariable TEXT NOT NULL,
    RHSVariableType TEXT REFERENCES VariableTypes_pk);
"""


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
    keys = ('ID',
            'Scheme_ID',
            'FileName',
            'Line',
            'FunctionIdentifier',
            'CFGNode',
            'Descriptors')
    matcher = re.compile('<sites unit="(?P<sig>[0-9a-f]{32})" scheme="(?P<sch>[a-zA-Z\-]*)">')

    Scheme_ID = None
    with file(filepath, 'r') as ifile:
        for ID, line in enumerate(ifile):
            match = matcher.match(line)
            if match:
                Scheme_ID = match.group('sch')
            elif line.startswith('</sites>'):
                # closing sites tag
                Scheme_ID = None
            else:
                vals = [ID, Scheme_ID] + line.strip().split(None, 4)
                yield dict(zip(keys, vals))


def process(sitesFile, dbname):
    con = sqlite3.connect(dbname)
    cursor = con.cursor()

    cursor.executescript(CreateQueries)

    for site in readSites(sitesFile):
        cursor.execute("INSERT INTO Sites VALUES(\
                            :ID, :FileName, :Line, :FunctionIdentifier,\
                            :CFGNode, :Scheme_ID);",
                        site)

        scheme = site['Scheme_ID']
        if schemeTables[scheme]:
            table = schemeTables[scheme]
            numCols = numTableColumns[scheme]
            entries = ', '.join('?' * numCols)

            query = "INSERT INTO %s VALUES (%s);" % (table, entries)
            cursor.execute(query, [site['ID']] + site['Descriptors'].split('\t'))

    con.commit()

def main():
    if len(sys.argv) != 3:
        print 'Usage: %s <sites-file> <sqlite-database>' % sys.argv[0]
    process(*sys.argv[1:])

if __name__ == '__main__': main()

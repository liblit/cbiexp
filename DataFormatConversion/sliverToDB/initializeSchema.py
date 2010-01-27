#!/s/python-2.5/bin/python -O

"""Initialize the database schema creating all relevant tables
"""

import sqlite3
import optparse
import os
import os.path
import sys

from DBConstants import EnumerationTables

def setupPragmas(conn, memory):
    """Setup SQLite pragmas.

    Args:
        memory: Cache size in MB
    """

    # Prepare SQLite PRAGMAs
    # NOTE: Most of these are listed here with their default values and only
    #       mentioned as objects of interest which may need to be tweaked in
    #       case of dismal DB performance etc. For more information as to what
    #       each PRAGMA affects see http://www.sqlite.org/pragma.html
    # TODO: Tweak PRAGMA values
    _sqlPragmas = '''
        PRAGMA foreign_keys = ON;
        PRAGMA default_cache_size = %d;
        PRAGMA count_changes = false;
        PRAGMA page_size = 1024;
        PRAGMA synchronous = FULL;
    ''' % (memory * 1024, )

    cursor = conn.cursor()

    # Set PRAGMAs
    map(cursor.execute, _sqlPragmas.split('\n'))

    conn.commit()
    cursor.close()




def setupTables(conn, version):
    """Create initial tables in database

    The setup step will not fail if the tables already exist

    Args:
        conn: sqlite3 connection object
        version: Schema version number to be implemented. Currently only
                supports version 1

    """

    # Get the cursor
    c = conn.cursor()

    # Prepare SQL commands -- Create tables
    _sqlQuery = '''
        CREATE TABLE
            SchemaVersion(
                SchemaID INTEGER NOT NULL
                        CONSTRAINT SchemaVersion_pk PRIMARY KEY);

        CREATE TABLE
            Outcomes(
                OutcomeID INTEGER NOT NULL
                            CONSTRAINT Outcomes_pk PRIMARY KEY ASC
                                AUTOINCREMENT,
                Name TEXT NOT NULL UNIQUE);

        CREATE TABLE
            Runs(
                RunID INTEGER NOT NULL
                        CONSTRAINT Runs_pk PRIMARY KEY,
                OutcomeID INTEGER NOT NULL,
                        CONSTRAINT Runs_OutcomeID_fk FOREIGN KEY (OutcomeID)
                            REFERENCES Outcomes(OutcomeID));

        CREATE TABLE
            Schemes(
                SchemeID INTEGER NOT NULL
                        CONSTRAINT Schemes_pk PRIMARY KEY ASC
                            AUTOINCREMENT,
                Name TEXT NOT NULL UNIQUE,
                SchemeCode TEXT NOT NULL UNIQUE);

        CREATE TABLE
            Fields(
                FieldID INTEGER NOT NULL
                        CONSTRAINT Fields_pk PRIMARY KEY ASC AUTOINCREMENT,
                SchemeID INTEGER NOT NULL,
                Name TEXT NOT NULL,
                CONSTRAINT Fields_SchemeID_fk FOREIGN KEY (SchemeID)
                    REFERENCES Schemes(SchemeID));

        CREATE TABLE
            Units(
                UnitID INTEGER NOT NULL
                        CONSTRAINT Units_pk PRIMARY KEY ASC AUTOINCREMENT,
                Signature TEXT NOT NULL,
                SchemeID INTEGER NOT NULL,
                CONSTRAINT Units_SchemeID_fk FOREIGN KEY (SchemeID)
                    REFERENCES Schemes(SchemeID),
                CONSTRAINT Units_un UNIQUE (Signature, SchemeID));

        CREATE TABLE
            AccessTypes(
                AccessTypeID INTEGER NOT NULL
                            CONSTRAINT AccessTypes_pk PRIMARY KEY ASC
                                AUTOINCREMENT,
                AccessType TEXT NOT NULL UNIQUE);

        CREATE TABLE
            AssignmentTypes(
                AssignmentTypeID INTEGER NOT NULL
                                CONSTRAINT AssignmentTypes_pk PRIMARY KEY ASC
                                    AUTOINCREMENT,
                AssignmentType TEXT NOT NULL UNIQUE);

        CREATE TABLE
            VariableTypes(
                VariableTypeID INTEGER NOT NULL
                                CONSTRAINT VariableTypes_pk PRIMARY KEY ASC
                                    AUTOINCREMENT,
                VariableType TEXT NOT NULL UNIQUE);

        CREATE TABLE
            Sites(
                SiteID INTEGER NOT NULL
                        CONSTRAINT Sites_pk PRIMARY KEY ASC AUTOINCREMENT,
                FileName TEXT NOT NULL,
                Line INTEGER NOT NULL,
                FunctionIdentifier TEST NOT NULL,
                CFGNode INTEGER NOT NULL,
                UnitID INTEGER NOT NULL,
                CONSTRAINT Sites_UnitID_fk FOREIGN KEY (UnitID)
                    REFERENCES Units(UnitID));

        CREATE TABLE
            AtomsSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT AtomsSiteDescriptors_pk PRIMARY KEY,
                Variable TEXT NOT NULL,
                AccessTypeID INTEGER NOT NULL,
                CONSTRAINT AtomsSiteDescriptors_SiteID_fk FOREIGN KEY (SiteID)
                    REFERENCES Sites(SiteID),
                CONSTRAINT AtomsSiteDescriptors_AccessTypeID_fk FOREIGN KEY
                    (AccessTypeID) REFERENCES AccessTypes(AccessTypeID));

        CREATE TABLE
            BoundsSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT BoundsSiteDescriptors_pk PRIMARY KEY,
                Variable TEXT NOT NULL,
                VariableTypeID INTEGER NOT NULL,
                AccessTypeID INTEGER NOT NULL,
                CONSTRAINT BoundsSiteDescriptors_SiteID_fk FOREIGN KEY (SiteID)
                    REFERENCES Sites(SiteID),
                CONSTRAINT BoundsSiteDescriptors_VariableTypeID_fk FOREIGN KEY
                    (VariableTypeID) REFERENCES VariableType(VariableTypeID),
                CONSTRAINT BoundsSiteDescriptors_AccessTypeID_fk FOREIGN KEY
                    (AccessTypeID) REFERENCES AccessType(AccessTypeID));

        CREATE TABLE
            BranchesSiteDescriptors(
                SiteId INTEGER NOT NULL
                        CONSTRAINT BranchesSiteDescriptors_pk PRIMARY KEY,
                Predicate TEXT NOT NULL,
                CONSTRAINT BranchesSiteDescriptors_SiteID_fk FOREIGN KEY
                    (SiteID) REFERENCES Sites(SiteID));

        CREATE TABLE
            FloatKindsSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT FloatKindsSiteDescriptors_pk PRIMARY KEY,
                Variable TEXT NOT NULL,
                VariableTypeID INTEGER NOT NULL,
                AccessTypeID INTEGER NOT NULL,
                CONSTRAINT FloatKindsSiteDescriptors_SiteID_fk FOREIGN KEY
                    (SiteID) REFERENCES Sites(SiteID),
                CONSTRAINT FloatKindsSiteDescriptors_VariableTypeID_fk FOREIGN
                    KEY (VariableTypeID) REFERENCES
                    VariableTypes(VariableTypeID),
                CONSTRAINT FloatKindsSiteDescriptors_AccessTypeID_fk FOREIGN
                    KEY (AccessTypeID) REFERENCES AccessTypes(AccessTypeID));

        CREATE TABLE
            GObjectUnrefSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT GObjectUnrefSiteDescriptors_pk PRIMARY KEY,
                OBJECT TEXT NOT NULL,
                CONSTRAINT GObjectUnrefSiteDescriptors_SiteID_fk FOREIGN
                    KEY (SiteID) REFERENCES Sites(SiteID));

        CREATE TABLE
            ReturnsSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT ReturnsSiteDescriptors_pk PRIMARY KEY,
                Callee TEXT NOT NULL,
                CONSTRAINT ReturnsSiteDescriptors_SiteID_fk FOREIGN KEY
                    (SiteID) REFERENCES Sites(SiteID));

        CREATE TABLE
            ScalarPairsSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT ScalarPairsSiteDescriptors_pk PRIMARY KEY,
                LHSVariable TEXT NOT NULL,
                LHSVariableTypeID INTEGER NOT NULL,
                LHSAssignmentTypeID INTEGER NOT NULL,
                RHSVariable TEXT NOT NULL,
                RHSVariableTypeID INTEGER NOT NULL,
                CONSTRAINT ScalarPairsSiteDescriptors_SiteID_fk FOREIGN KEY
                    (SiteID) REFERENCES Sites(SiteID),
                CONSTRAINT ScalarPairsSiteDescriptors_LHSVariableTypeID_fk
                    FOREIGN KEY (LHSVariableTypeID) REFERENCES
                    VariableTypes(VariableTypeID),
                CONSTRAINT ScalarPairsSiteDescriptors_LHSAssignmentTypeID_fk
                    FOREIGN KEY (LHSAssignmentTypeID) REFERENCES
                    AssignmentTypes(AssignmentTypeID),
                CONSTRAINT ScalarPairsSiteDescriptors_RHSVariableTypeID_fk
                    FOREIGN KEY (RHSVariableTypeID) REFERENCES
                    VariableTypes(VariableTypeID));

        CREATE TABLE
            CompareSwapSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT CompareSwapSiteDescriptors_pk PRIMARY KEY,
                Variable TEXT NOT NULL,
                CONSTRAINT CompareSwapSiteDescriptors_SiteID_fk FOREIGN KEY
                    (SiteID) REFERENCES Sites(SiteID));

        CREATE TABLE
            Plans(
                PlanID INTEGER NOT NULL
                    CONSTRAINT Plans_pk PRIMARY KEY);

        CREATE TABLE
            RunsInPlans(
                PlanID INTEGER NOT NULL,
                RunID INTEGER NOT NULL UNIQUE,
                CONSTRAINT RunsInPlans_PlanID_fk FOREIGN KEY (PlanID)
                    REFERENCES Plans(PlanID),
                CONSTRAINT RunInPlans_RunID_fk FOREIGN KEY (RunID)
                    REFERENCES Runs(RunID));

        CREATE TABLE
            SitesInPlans(
                PlanID INTEGER NOT NULL,
                SiteID INTEGER NOT NULL,
                CONSTRAINT SitesInPlans_PlanID_fk FOREIGN KEY (PlanID)
                    REFERENCES Plans(PlanID),
                CONSTRAINT SitesInPlans_SiteID_fk FOREIGN KEY (SiteID)
                    REFERENCES Sites(SiteID),
                CONSTRAINT SitesInPlans_un UNIQUE (PlanID, SiteID));

        CREATE TABLE
            SampleValues(
                SiteID INTEGER NOT NULL,
                FieldID INTEGER NOT NULL,
                RunID INTEGER NOT NULL,
                Value INTEGER NOT NULL,
                Phase INTEGER DEFAULT -1,
                CONSTRAINT SampleValues_SiteID_fk FOREIGN KEY (SiteID)
                    REFERENCES Sites(SiteID),
                CONSTRAINT SampleValues_FieldID_fk FOREIGN KEY (FieldID)
                    REFERENCES Fields(FieldID),
                CONSTRAINT SampleValues_RunID_fk FOREIGN KEY (RunID)
                    REFERENCES Runs(RunID),
                CONSTRAINT SampleValues_un UNIQUE (SiteID, FieldID, RunID,
                    Phase));

        CREATE TABLE
            SampleCounts(
                SiteID INTEGER NOT NULL,
                FieldID INTEGER NOT NULL,
                RunID INTEGER NOT NULL,
                Count INTEGER NOT NULL,
                Phase INTEGER DEFAULT -1,
                CONSTRAINT SampleCounts_SiteID_fk FOREIGN KEY (SiteID)
                    REFERENCES Sites(SiteID),
                CONSTRAINT SampleCounts_FieldID_fk FOREIGN KEY (FieldID)
                    REFERENCES Fields(FieldID),
                CONSTRAINT SampleCounts_RunID_fk FOREIGN KEY (RunID)
                    REFERENCES Runs(RunID),
                CONSTRAINT SampleCounts_un UNIQUE (SiteID, FieldID, RunID,
                    Phase),
                CONSTRAINT SampleCounts_chk CHECK (Count > 0));
    '''
    # NOTE: The UNIQUE constraint if applied on a set of columns one more of
    #       which may have null values (the 'Phase' column in the case of
    #       SampleCounts) will not work as intended for most implementations of
    #       SQL (eg. SQLite, PostGreSQL etc.), in fact it would only function
    #       as intended with Informix and Microsoft SQL Server. As a workaround
    #       when phases aren't being used we denote this by having a default
    #       value of '-1' inserted into the appropriate column.


    # Create TABLES
    map(c.execute, _sqlQuery.split(';'))

    # Initialize SchemaVersion
    if isTableEmpty(conn, 'SchemaVersion'):
        c.execute('INSERT INTO SchemaVersion (SchemaID) Values (1)')

    # Initialize Enumeration tables
    for table in EnumerationTables:
        SafeInsert(conn, table, EnumerationTables[table])

    # Save and commit
    conn.commit()

    # Close cursor
    c.close()


def SafeInsert(conn, table, data, commit=False):
    """Safely insert values into enumeration tables.

        ARGUMENTS:
            conn: sqlite3 connection object
            table: Name of the table
            data: List of row-tuples to fill the table with
            commit: Commits the insert if True

        RETURNS:
            True: If data was inserted
            False: If test failed and no data was inserted
    """
    if isTableEmpty(conn, table):
        c = conn.cursor()
        entries = ', '.join('?' * len(data[0]))
        query = 'INSERT INTO %s VALUES (%s);' % (table, entries)

        for t in data:
            c.execute(query, t)

        if commit:
            conn.commit()

        c.close()
        return True

    else:
        return False



def isTableEmpty(conn, table):
    """Check if a table is empty.

        ARGUMENTS:
            conn: sqlite3 connection object
            table: Name of the table

        RETURNS:
            bool
    """
    c = conn.cursor()
    c.execute('SELECT COUNT(*) FROM ' + table)
    r = c.fetchall()
    if r[0][0] == 0:
        return True
    else:
        return False



def main(argv=None):
    """Generate an sqlite database and creating any tables which may not
        already exist
    """
    if argv is None:
        argv = sys.argv

    parser = optparse.OptionParser(usage='%prog [options] <database>.')
    parser.add_option('-f', '--force', action='store_true', default=False,
                      help = 'append to existing database')
    parser.add_option('-v', '--version', action='store', default=1, type='int',
                      help = 'version of schema to implement')

    parser.add_option('-m', '--memory', action='store', type='int', default=20,
                      help = 'SQLite cache size in MB')

    options, args = parser.parse_args(argv[1:])
    if len(args) != 1: parser.error('wrong number of positional arguments')

    cbi_db = args[0]
    if os.path.exists(cbi_db):
        if not options.force:
            return 'Use --force to re-initialize a pre-existing database.'
        else:
            os.remove(cbi_db)

    conn = sqlite3.connect(cbi_db)

    setupPragmas(conn, options.memory)
    setupTables(conn, options.version)

    conn.close()


if __name__ == '__main__':
    sys.exit(main())

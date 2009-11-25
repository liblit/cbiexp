#!/s/python-2.5/bin/python -O

"""Initialize the database schema creating all relevant tables
"""

import sqlite3
import optparse
import os
import os.path
import sys

##############################################################################
# Enumeration Tables
##############################################################################

EnumerationTables = {
    'Outcomes':[
        (0, 'SUCCESS'),
        (1, 'FAILURE'),
        (2, 'UNKNOWN'),
    ],
 
    'Schemes':[
        (1, 'atoms', 'A',),
        (2, 'fun-reentries', 'Z',),
        (3, 'compare-swap', 'C',),
        (4, 'atoms-rw', 'W'),
        (5, 'branches', 'B',),
        (6, 'returns', 'R',),
        (7, 'scalar-pairs', 'S',),
        (8, 'float-kinds', 'F',),
        (9, 'bounds', 'D',),
        (10, 'g-object-unref', 'G',),
        (11, 'function-entries', 'E'),
    ],

    'Fields':[
        (1, 1, 'SAME_THREAD_PERFORMED_LAST_MEM_ACCESS'),
        (2, 1, 'DIFFERENT_THREAD_PERFORMED_LAST_MEM_ACCESS'),
        (3, 2, 'SINGLE_THREAD_EXECUTING_FUNCTION'),
        (4, 2, 'MULTIPLE_THREADS_EXECUTING_FUNCTION'),
        (5, 3, 'SAME_VALUE_OBSERVED'),
        (6, 3, 'DIFFERENT_VALUE_OBSERVED'),
        (7, 4, 'SAME_THREAD_PERFORMED_LAST_MEM_ACCESS'),
        (8, 4, 'DIFFERENT_THREAD_PERFORMED_LAST_MEM_ACCESS'),
        (9, 5, 'FALSE'),
        (10, 5, 'TRUE'),
        (11, 6, 'IS_NEGATIVE'),
        (12, 6, 'IS_ZERO'),
        (13, 6, 'IS_POSITIVE'),
        (14, 7, 'IS_LESS_THAN'),
        (15, 7, 'IS_EQUAL_TO'),
        (16, 7, 'IS_GREATER_THAN'),
        (17, 8, 'IS_NEGATIVE_INF'),
        (18, 8, 'IS_NEGATIVE_AND_NORMALIZED'),
        (19, 8, 'IS_NEGATIVE_AND_DENORMALIZED'),
        (20, 8, 'IS_NEGATIVE_ZERO'),
        (21, 8, 'IS_NAN'),
        (22, 8, 'IS_POSITIVE_ZERO'),
        (23, 8, 'IS_POSITIVE_AND_DENORMALIZED'),
        (24, 8, 'IS_POSITIVE_AND_NORMALIZED'),
        (25, 8, 'IS_POSITIVE_INF'),
        (26, 9, 'MINIMUM_VALUE'),
        (27, 9, 'MAXIMUM_VALUE'),
        (28, 10, 'ZERO_REFERENCES'),
        (29, 10, 'ONE_REFERENCE'),
        (30, 10, 'MORE_THAN_ONE_REFERENCE'),
        (31, 10, 'INVALID'),
        (32, 11, 'FUNCTION_ENTERED'),
    ],


    'AccessTypes': [ 
        (1, 'read'),
        (2, 'write'),
    ],
 
    'AssignmentTypes':[ 
        (1, 'direct'),
        (2, 'field'),
        (3, 'index'),
    ],

    'VariableTypes':[
        (1, 'local'),
        (2, 'global'),
        (3, 'mem'),
        (4, 'const'),
    ],
}

### END OF Enumeration Tables



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
    _sqlCommand = '''
        CREATE TABLE IF NOT EXISTS
            SchemaVersion(
                SchemaID INTEGER NOT NULL
                        CONSTRAINT SchemaVersion_pk PRIMARY KEY);
        
        CREATE TABLE IF NOT EXISTS
            Outcomes(
                OutcomeID INTEGER NOT NULL 
                            CONSTRAINT Outcomes_pk PRIMARY KEY ASC 
                                AUTOINCREMENT,
                Name TEXT NOT NULL UNIQUE);

        CREATE TABLE IF NOT EXISTS
            Runs(
                RunID INTEGER NOT NULL
                        CONSTRAINT Runs_pk PRIMARY KEY, 
                OutcomeID INTEGER NOT NULL,
                        CONSTRAINT Runs_OutcomeID_fk FOREIGN KEY (OutcomeID)
                            REFERENCES Outcomes(OutcomeID));

        CREATE TABLE IF NOT EXISTS
            Schemes(
                SchemeID INTEGER NOT NULL
                        CONSTRAINT Schemes_pk PRIMARY KEY ASC
                            AUTOINCREMENT,
                Name TEXT NOT NULL UNIQUE,
                SchemeCode TEXT NOT NULL UNIQUE);

        CREATE TABLE IF NOT EXISTS
            Fields(
                FieldID INTEGER NOT NULL
                        CONSTRAINT Fields_pk PRIMARY KEY ASC AUTOINCREMENT,
                SchemeID INTEGER NOT NULL,
                Name TEXT NOT NULL,
                CONSTRAINT Fields_SchemeID_fk FOREIGN KEY (SchemeID)
                    REFERENCES Schemes(SchemeID));

        CREATE TABLE IF NOT EXISTS
            AccessTypes(
                AccessTypeID INTEGER NOT NULL
                            CONSTRAINT AccessTypes_pk PRIMARY KEY ASC
                                AUTOINCREMENT,
                AccessType TEXT NOT NULL UNIQUE);

        CREATE TABLE IF NOT EXISTS
            AssignmentTypes(
                AssignmentTypeID INTEGER NOT NULL
                                CONSTRAINT AssignmentTypes_pk PRIMARY KEY ASC
                                    AUTOINCREMENT,
                AssignmentType TEXT NOT NULL UNIQUE);

        CREATE TABLE IF NOT EXISTS
            VariableTypes(
                VariableTypeID INTEGER NOT NULL
                                CONSTRAINT VariableTypes_pk PRIMARY KEY ASC
                                    AUTOINCREMENT,
                VariableType TEXT NOT NULL UNIQUE);

        CREATE TABLE IF NOT EXISTS
            Sites(
                SiteID INTEGER NOT NULL
                        CONSTRAINT Sites_pk PRIMARY KEY ASC AUTOINCREMENT,
                FileName TEXT NOT NULL,
                Line INTEGER NOT NULL,
                FunctionIdentifier TEST NOT NULL,
                CFGNode INTEGER NOT NULL,
                SchemeID INTEGER NOT NULL,
                CONSTRAINT Sites_SchemeID_fk FOREIGN KEY (SchemeID)
                    REFERENCES Schemes(SchemeID));

        CREATE TABLE IF NOT EXISTS
            AtomsSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT AtomsSiteDescriptors_pk PRIMARY KEY,
                Variable TEXT NOT NULL,
                AccessTypeID INTEGER NOT NULL,
                CONSTRAINT AtomsSiteDescriptors_SiteID_fk FOREIGN KEY (SiteID)
                    REFERENCES Sites(SiteID),
                CONSTRAINT AtomsSiteDescriptors_AccessTypeID_fk FOREIGN KEY
                    (AccessTypeID) REFERENCES AccessTypes(AccessTypeID));

        CREATE TABLE IF NOT EXISTS
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
       
        CREATE TABLE IF NOT EXISTS
            BranchesSiteDescriptors(
                SiteId INTEGER NOT NULL
                        CONSTRAINT BranchesSiteDescriptors_pk PRIMARY KEY,
                Predicate TEXT NOT NULL,
                CONSTRAINT BranchesSiteDescriptors_SiteID_fk FOREIGN KEY
                    (SiteID) REFERENCES Sites(SiteID));

        CREATE TABLE IF NOT EXISTS
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

        CREATE TABLE IF NOT EXISTS
            GObjectUnrefSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT GObjectUnrefSiteDescriptors_pk PRIMARY KEY,
                OBJECT TEXT NOT NULL,
                CONSTRAINT GObjectUnrefSiteDescriptors_SiteID_fk FOREIGN
                    KEY (SiteID) REFERENCES Sites(SiteID));

        CREATE TABLE IF NOT EXISTS
            ReturnsSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT ReturnsSiteDescriptors_pk PRIMARY KEY,
                Callee TEXT NOT NULL,
                CONSTRAINT ReturnsSiteDescriptors_SiteID_fk FOREIGN KEY
                    (SiteID) REFERENCES Sites(SiteID));

        CREATE TABLE IF NOT EXISTS
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
        
        CREATE TABLE IF NOT EXISTS
            CompareSwapSiteDescriptors(
                SiteID INTEGER NOT NULL
                        CONSTRAINT CompareSwapSiteDescriptors_pk PRIMARY KEY,
                Variable TEXT NOT NULL,
                CONSTRAINT CompareSwapSiteDescriptors_SiteID_fk FOREIGN KEY
                    (SiteID) REFERENCES Sites(SiteID));

        CREATE TABLE IF NOT EXISTS
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

        CREATE TABLE IF NOT EXISTS
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
    map(c.execute, _sqlCommand.split(';'))
   
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
    """Generate an sqlite database and creating any tables which may not already
        exist
    """
    if argv is None:
        argv = sys.argv

    parser = optparse.OptionParser(usage='%prog [options] <database>.')
    parser.add_option('-f', '--force', action='store_true', default=False, 
                      help = 'append to existing database')
    parser.add_option('-v', '--version', action='store', default=1,
                      help = 'version of schema to implement')
    
    options, args = parser.parse_args(argv[1:])
    if len(args) != 1: parser.error('wrong number of positional arguments')
    
    cbi_db = args[0]
    if os.path.exists(cbi_db):
        if not options.force:
            return 'Use --force to append to a pre-existing database.'
        else:
            conn = sqlite3.connect(cbi_db)
            c = conn.cursor()
            c.execute('SELECT SchemaID from SchemaVersion')
            r = c.fetchall()
            c.close()
            if (len(r) != 1) or (r[0][0] != 1):
                return ('Incompatible schema version in pre-existing ' +
                        'database')
    else:
        conn = sqlite3.connect(cbi_db)
    
    setupTables(conn, options.version)
    

if __name__ == '__main__':
    sys.exit(main())

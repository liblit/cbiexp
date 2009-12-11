""" Static enumerations and mappings used in the database """

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

# Schemes whose instrumentation records some type of values
# instead of incrementing a counter.  Values go to the
# 'SampleValues' tables and counters to 'SampleCounters'.
# Also, zero count are not inserted into the table.
ValueRecordingSchemes = frozenset(('bounds',))


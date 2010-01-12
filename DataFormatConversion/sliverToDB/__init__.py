import sqlite3
import os

import utils

from initializeSchema import setupTables
from readCountsFile import processCountsFile
from readReports import processReports
from runsIntoDB import processLabels, processOutcomesFile
from sitesIntoDB import writeSitesIntoDB


def setupDatabase(cbi_db, version=1):
    if version != 1:
        raise ValueError('Incompatible version %d' % version)

    if os.path.exists(cbi_db):
        raise ValueError('File exists: %s' % cbi_db)

    conn = sqlite3.connect(cbi_db)
    setupTables(conn, version)

    conn.close()

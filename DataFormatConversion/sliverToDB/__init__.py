import sqlite3
import os

from initializeSchema import setupTables
from readCountsFile import writeSamplesIntoDB
from readReports import processReportFiles
from runsIntoDB import writeRunsIntoDB
from sitesIntoDB import writeSitesIntoDB


def setupDatabase(cbi_db, sitesTxt, outcomesTxt, version=1)
    if version != 1:
        raise ValueError('Incompatible version %d' % version)

    if os.path.exists(cbi_db):
        raise ValueError('File exists: %s' % cbi_db)

    conn = sqlite3.connect(cbi_db)

    setupTables(conn, version)
    writeSitesIntoDB(conn, sitesTxt, version)
    writeRunsIntoDB(conn, outcomesTxt, version)

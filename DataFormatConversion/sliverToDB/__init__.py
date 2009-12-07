import sqlite3
import os

from initializeSchema import setupTables
from runsAndSamplesIntoDB import writeRunsIntoDB, writeSamplesIntoDB
from sitesIntoDB import writeSitesIntoDB


def sliverToDB(cbi_db, sitesTxt, outcomesTxt, countsTxt, phase=None, version=1):
    if version != 1:
        raise ValueError('Incompatible version %d' % version)

    if os.path.exists(cbi_db):
        raise ValueError('File exists: %s' % cbi_db)

    conn = sqlite3.connect(cbi_db)

    setupTables(conn, version)
    writeSitesIntoDB(conn, sitesTxt, version)
    writeRunsIntoDB(conn, outcomesTxt, version)
    writeSamplesIntoDB(conn, countsTxt, phase, version)

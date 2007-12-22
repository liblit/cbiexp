#!/s/std/bin/python -O 

######################################
#
# Authors Lydia Sidrak, Jason Fletchall, 11/2007
#
# This script contains a main() to run the visualization
# analysis on each distribution-release.
#
# Usage: main.py resultsRootDir
#
# Example: main.py /tmp/fletchal/results
#
######################################
 
import sys
import os
import string
import commands
from pyPgSQL import PgSQL
import visualize_one

debug = 0

######################################
#
# Gets a list of all interesting releases
# from the database, then calls
# visualize_one on all of them
#
######################################
def main():

    #Abort if user passed in wrong number of arguments
    if len(sys.argv) != 2:
        print "Usage: main.py resultsRootDir"
        print "Example: main.py /tmp/fletchal/results"
        return
    
    resultsRoot = sys.argv[1]
    if not resultsRoot.endswith("/"):
        resultsRoot += "/"

    # Do query for master list of failed runs
    db = PgSQL.connect(host='postgresql.cs.wisc.edu', port=49173, database='cbi')
    db.autocommit = False

    query = "SELECT application_name, application_version, application_release, build_distribution, COUNT(*) AS \"failed_runs\" FROM run NATURAL JOIN build WHERE exit_signal <> 0 " + "GROUP BY application_name, application_version, application_release, build_distribution" + " HAVING COUNT(*) > 1"

    cursor = db.cursor()
    cursor.execute(query)
    rows = cursor.fetchall()

    for name, version, release, distribution, failed_runs in rows:
                      
        folder = name+"-"+version+"-"+release
        homeDir = resultsRoot+distribution+"/"+folder

        #Make sure that the current distribution/release exists in the file system, and that it contains the expected folder structure
        if distribution in os.listdir(resultsRoot) and folder in os.listdir(resultsRoot+distribution) and "sites" in os.listdir(homeDir) and "data" in os.listdir(homeDir) and "src" in os.listdir(homeDir):
        
            sys.argv = "junk", "-esp", homeDir
            visualize_one.main()

        else:
            print "Could not find results directory", homeDir, "or it is missing sites and/or data and/or src directories.  Skipping this release."
        
    db.rollback()
    db.close()
    outfile.close()


if __name__ == '__main__':
    main()

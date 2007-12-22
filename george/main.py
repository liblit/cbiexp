#!/s/std/bin/python -O 

######################################
#
# Authors Lydia Sidrak, Jason Fletchall, 11/2007
#
# This script contains a main() to run the visualization
# analysis on each distribution-release.
#
# Usage: main.py resultsRootDir srcHtmlRootDir
#
# Example: main.py /tmp/fletchal/results /tmp/fletchal/copy
#
######################################
 
import sys
import os
import string
import commands
from pyPgSQL import PgSQL
from make_menu import make_menu
sys.path.append("./statistics")
import calculate_statistics
import generate_plots
import visualize_one

debug = 2

######################################
#
# Gets a list of all interesting releases
# from the database, then calls
# calculate-statistics on all of them
#
######################################
def main():

    #Abort if user passed in wrong number of arguments
    if len(sys.argv) != 3:
        print "Usage: main.py resultsRootDir srcHtmlRootDir"
        print "Example: main.py /tmp/fletchal/results /tmp/fletchal/copy"
        return
    
    oldRoot = sys.argv[1]
    srcHtmlRootDir = sys.argv[2]

    #html_root

    # Do query for master list of failed runs
    db = PgSQL.connect(host='postgresql.cs.wisc.edu', port=49173, database='cbi')
    db.autocommit = False

    query = "SELECT application_name, application_version, application_release, build_distribution, COUNT(*) AS \"failed_runs\" FROM run NATURAL JOIN build WHERE exit_signal <> 0 " + "GROUP BY application_name, application_version, application_release, build_distribution" + " HAVING COUNT(*) > 1"
 
    # Create navigation
    #outfile = open("home.html", "w")

    cursor = db.cursor()
    cursor.execute(query)
    rows = cursor.fetchall()
    #outfile.write("<table cellpadding=\"2\" cellspacing =\"1\" border =\"1\">")

    for name, version, release, distribution, failed_runs in rows:
                      
        folder = name+"-"+version+"-"+release
        
        reportsDir = oldRoot+"/"+distribution+"/"+folder+"/data"
        sitesDir = oldRoot+"/"+distribution+"/"+folder+"/sites"
        vizOutDir = "/tmp/viz/"+distribution+"/"+folder
        srcHtmlDir = srcHtmlRootDir+"/"+distribution+"/"+folder+"/src/"+name+"-"+version
        distribution = distribution
        release_arg = folder

        #Make sure that the current distribution/release exists in the file system
        if distribution in os.listdir(oldRoot) and folder in os.listdir(oldRoot+"/"+distribution) and "sites" in os.listdir(oldRoot+"/"+distribution+"/"+folder+"/") and "data" in os.listdir(oldRoot+"/"+distribution+"/"+folder+"/") and "src" in os.listdir(oldRoot+"/"+distribution+"/"+folder+"/"):
        
            sys.argv ="junk",reportsDir,sitesDir,vizOutDir,srcHtmlDir,distribution,release_arg
            visualize_one.main()

        else:
            print "Could not find results directory", oldRoot+distribution+"/"+folder+"/", "or it is missing sites and/or data and/or src directories.  Skipping this release."
    #outfile.write("</table>")
        
    db.rollback()
    db.close()
    outfile.close()


if __name__ == '__main__':
    main()

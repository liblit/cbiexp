#!/s/std/bin/python -O

######################################
#
# Author: Jason Fletchall, 12/2007
#
# This script creates an overview page of the
# analyses for all application-releases in the
# given directory.

# Usage: create_overview_page.py analysisDir overviewPageOutDir
#
######################################


import os
import sys
import commands

#sys.path.append("../george")
#import visualize_one

debug = 2


######################################
#
# Creates the final HTML document with
# links to each distribution-release
#
######################################
def createOverviewPage(linksArray, overviewPageOutDir):

    #Opens the final html file for writing
    overviewPage = open(overviewPageOutDir+"/overview.html", "w")

    #Generates some opening html tags
    overviewPage.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"' +
            '"http://www.w3.org/TR/html4/loose.dtd">' +
            '<html>')

    #Generates a header
    overviewPage.write('<head>' +
                        '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">' +
                        '<title>CBI Overview</title>' +
                        '</head>')

    #Outputs links to each interesting distribution/release
    overviewPage.write('<body>')
    for distribution in linksArray:
        overviewPage.write('<ul>')
        overviewPage.write('<lh><font size="4"><strong>' + distribution + '</strong></font></lh>')
        for release in linksArray[distribution]:
            overviewPage.write('<li>' + linksArray[distribution][release][0])
            if linksArray[distribution][release][1] > 0:
                overviewPage.write('<img src="favicon.png"></img>' + '<strong>' + str(linksArray[distribution][release][1]) + '</strong></li>')
        overviewPage.write('</ul>')
    overviewPage.write('</body>')

    #Closes the document
    overviewPage.write('</html>')
    overviewPage.close()

    print "Wrote", overviewPageOutDir+"/overview.html"            

######################################
#
# Generates links for each summary page
# in given summary array, and returns them
# in the same array.
#
######################################
def generateLinks(summaryArray, analysisDir):

    for distribution in summaryArray:
        for release in summaryArray[distribution]:
            summaryArray[distribution][release][0] = '<a href="' + analysisDir + '/' + distribution + '/' + release + '/summary.xml">' + release + '</a>'

    return summaryArray

    

######################################
#
# Gets all the summary pages in the given
# directory, and returns those in an array
# indexed by build distribution, release.
#
######################################
def getSuccessfulSummaryPages(analysisDir, undo):

    #Walks the given directory for summary.xml files
    summaryArray = {}
    for analysisPath, dirs, resultFiles in os.walk(analysisDir):
        #Looking in any reports, src, or sites directories is a waste of time
        if 'data' in dirs:
            dirs.remove('data')
        if 'debug' in dirs:
            dirs.remove('debug')
        if 'sites' in dirs:
            dirs.remove('sites')
        if 'src' in dirs:
            dirs.remove('src')
        #if len(analysisDir.split('/')) == 5:

        for resultFile in resultFiles:
            if (resultFile == "summary.xml"):
                #Look in directory containing summary (analysis home) for preds.txt, count # predicates (if any)
                preds = open(analysisPath + "/preds.txt", "r")
                predList = preds.readlines()
                numPreds = len(predList)
                
                #All paths take the form .../distribution/release/, so extract distribution & release names
                splitPath = analysisPath.rsplit("/", 2)
                distribution = splitPath[1]
                release = splitPath[2]
                if distribution not in summaryArray:
                    summaryArray[distribution] = {}
                summaryArray[distribution][release] = ["summary.xml", numPreds]

                #Backup src htmls if requested
                if backup == True:
                    createBackup(analysisPath)
                #Process html views to pretty them up
                #highlightSource(analysisPath)
                if undo == True:
                    undoHighlight(analysisPath)

                #Update links in predicate table to make what they link to more readable
                processPredicateLinks(analysisPath)
                
                print "Found summary page for", distribution + "/" + release

    return summaryArray


######################################
#
# Highlights all the src html files using
# George.
#
######################################
def highlightSource(resultsDir):
    sys.argv = "junk","-esp",resultsDir
    visualize_one.main()


######################################
#
# "Undoes" the html highlighting done by
# George.  This is really just copying
# all the html files back from a backup
# directory.
#
######################################
def undoHighlight(resultsDir):
    if not resultsDir.endswith("/"):
        resultsDir += "/"
    copyFromDir = resultsDir + "srcBACKUP/"
    copyToDir = resultsDir + "src/"
    commands.getoutput("../george/utils/copy_dir_files " + copyFromDir + " " + copyToDir)


######################################
#
# Creates a backup of all the src html
# files and stores them in srcBACKUP/
#
######################################
def createBackup(resultsDir):
    if not resultsDir.endswith("/"):
        resultsDir += "/"
    copyToDir = resultsDir + "srcBACKUP/"
    copyFromDir = resultsDir + "src/"
    commands.getoutput("../george/utils/copy_dir_files " + copyFromDir + " " + copyToDir)
    

def main():

    ##TODO
    # 1. Copy all things of interest to given output directory (summary.xml, supporting xmls, debug dir, etc.)
    # 2. Overview should go at top of that

    if len(sys.argv) != 3 and len(sys.argv) != 4:
        print "Usage: create_overview_page.py analysisDir overviewPageOutDir [-u -b]"
        print "Where -u indicates that George highlighting should be undone."
        print "Where -b indicates that a backup of src html should be made."
        return

    analysisDir = sys.argv[1]
    overviewPageOutDir = sys.argv[2]
    undo = False
    backup = False
    if len(sys.argv) == 4:
        option = sys.argv[3]
        if option == "-u":
            undo = True
        elif option == "-b":
            backup = True

    summaryArray = getSuccessfulSummaryPages(analysisDir, undo, backup)
    linksArray = generateLinks(summaryArray, analysisDir)
    createOverviewPage(linksArray, overviewPageOutDir)

    if debug > 1:
        commands.getoutput("firefox " + overviewPageOutDir + "/overview.html")


if __name__ == '__main__':
    main()

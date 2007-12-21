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

from pyPgSQL import PgSQL

import os
import sys
import commands

sys.path.append("/afs/cs.wisc.edu/u/f/l/fletchal/CBI/vizProject")
import visualize_one

debug = 2


######################################
#
# Returns the number of predictors
# listed in the given file
#
######################################
def countPreds(predsXML):

    predsFile = open(predsXML, "r")
    predList = predsFile.read()
    numPreds = predList.count('</predictor>')
    predsFile.close()
    return numPreds



######################################
#
# Creates an HTML document with
# links to each of the given distribution's
# app-releases
#
######################################
def createDistPage(indexedBuilds, overviewPageOutDir, distribution):

    #Opens the app/dist's html index file for writing
    overviewPage = open(overviewPageOutDir + distribution + "/index.html", "w")

    #Generates some opening html tags
    overviewPage.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"' +
            '"http://www.w3.org/TR/html4/loose.dtd">' +
            '<html>')

    #Generates a header
    overviewPage.write('<head>' +
                        '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">' +
                        '<title>CBI Distribution Overview - ' + distribution + '</title>' +
                        '<link href="file://' + overviewPageOutDir + 'releases.css" rel="stylesheet" type="text/css"/>' +
                        '<a href="' + overviewPageOutDir + 'distOverview.html"> Up </a>' +  
                        '</head>')

    #Outputs [application, version, release, PLDI2005 link, George link] in order of build date in table format
    overviewPage.write('<body>\n')

    overviewPage.write('<table border="1">\n')
    overviewPage.write('<tr><th> Application </th><th> Version </th><th> Release </th><th> Predicate View </th><th> George View </th></tr>\n')
    
    recentBuilds = flatten(indexedBuilds)
    mostRecentBuilds = getMostRecentDistBuilds(recentBuilds)
    
    for appName in sorted(mostRecentBuilds, reverse=True):
        version = mostRecentBuilds[appName][2]
        release = mostRecentBuilds[appName][3]
        pldiLink = '<img src="' + overviewPageOutDir + 'grayedFavicon.png"></img>'
        if distribution in os.listdir(overviewPageOutDir) and (appName+'-'+version+'-'+release) in os.listdir(overviewPageOutDir+distribution) and 'all_hl_corrected-exact-complete.xml' in os.listdir(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release):
            numPreds = countPreds(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release+'/all_hl_corrected-exact-complete.xml')
            if numPreds > 0:
                pldiLink = ''+str(numPreds)+'&nbsp;&nbsp;<a href="' + overviewPageOutDir + distribution + '/' + appName+'-'+version+'-'+release + '/all_hl_corrected-exact-complete.xml"><img src="' + overviewPageOutDir + 'favicon.png"></img></a> &nbsp;&nbsp;'
        if distribution in os.listdir(overviewPageOutDir) and (appName+'-'+version+'-'+release) in os.listdir(overviewPageOutDir+distribution) and 'src' in os.listdir(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release) and 'index.html' in os.listdir(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release+'/src'):
            georgeLink = '<a href="' + overviewPageOutDir + distribution + '/' + appName+'-'+version+'-'+release + '/src/index.html"><img src="' + overviewPageOutDir + 'georgeicon.png"></img></a>'
        else:
            georgeLink = '<img src="' + overviewPageOutDir + 'grayedGeorgeicon.png"></img>'

        version_release = (version+'-'+release).replace('.','-')
        overviewPage.write('<tr id="' + appName + '-' + version_release + '">' +
                           '<td>' + appName + '</td><td>' + version + '</td><td>' + release + '</td>' +
                           '<td align="center" height="40" valign="middle">' + pldiLink + '</td>' +
                           '<td align="center">' + georgeLink + '</td>' +
                           '</tr>\n')

    overviewPage.write('<br/><br/>\n')
    overviewPage.write('<table border="1">\n')
    overviewPage.write('<tr><th> Application </th><th> Version </th><th> Release </th><th> Predicate View </th><th> George View </th></tr>\n')

    for month in sorted(indexedBuilds, reverse=True):
        for day in sorted(indexedBuilds[month], reverse=True):
            for appName in sorted(indexedBuilds[month][day], reverse=True):
                distribution = indexedBuilds[month][day][appName][0]
                version = indexedBuilds[month][day][appName][0]
                release = indexedBuilds[month][day][appName][1]
                pldiLink = '<img src="' + overviewPageOutDir + 'grayedFavicon.png"></img>'
                if distribution in os.listdir(overviewPageOutDir) and (appName+'-'+version+'-'+release) in os.listdir(overviewPageOutDir+distribution) and 'all_hl_corrected-exact-complete.xml' in os.listdir(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release):
                    numPreds = countPreds(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release+'/all_hl_corrected-exact-complete.xml')
                    if numPreds > 0:
                        pldiLink = ''+str(numPreds)+'&nbsp;&nbsp;<a href="' + overviewPageOutDir + distribution + '/' + appName+'-'+version+'-'+release + '/all_hl_corrected-exact-complete.xml"><img src="' + overviewPageOutDir + 'favicon.png"></img></a> &nbsp;&nbsp;'
                if distribution in os.listdir(overviewPageOutDir) and (appName+'-'+version+'-'+release) in os.listdir(overviewPageOutDir+distribution) and 'src' in os.listdir(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release) and 'index.html' in os.listdir(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release+'/src'):
                    georgeLink = '<a href="' + overviewPageOutDir + distribution + '/' + appName+'-'+version+'-'+release + '/src/index.html"><img src="' + overviewPageOutDir + 'georgeicon.png"></img></a>'
                else:
                    georgeLink = '<img src="' + overviewPageOutDir + 'grayedGeorgeicon.png"></img>'

                    version_release = (version+'-'+release).replace('.','-')
                    overviewPage.write('<tr id="' + appName + '-' + version_release + '">' +
                                       '<td>' + appName + '</td><td>' + version + '</td><td>' + release + '</td>' +
                                       '<td align="center" height="40" valign="middle">' + pldiLink + '</td>' +
                                       '<td align="center">' + georgeLink + '</td>' +
                                       '</tr>\n')
            
    overviewPage.write('</table>')
    overviewPage.write('</body>')

    #Closes the document
    overviewPage.write('</html>')
    overviewPage.close()

    print "Wrote", overviewPageOutDir + appName + "/index.html"


    
######################################
#
# Creates the final HTML document with
# links to each distribution-release
#
######################################
def createAppPage(indexedBuilds, overviewPageOutDir, appName):

    #Opens the app/dist's html index file for writing
    if appName not in os.listdir(overviewPageOutDir):
        commands.getoutput("mkdir -p " + overviewPageOutDir + appName)
    overviewPage = open(overviewPageOutDir + appName + "/index.html", "w")

    #Generates some opening html tags
    overviewPage.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"' +
            '"http://www.w3.org/TR/html4/loose.dtd">' +
            '<html>')

    #Generates a header
    overviewPage.write('<head>' +
                        '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">' +
                        '<title>CBI Application Overview - ' + appName + '</title>' +
                        '<link href="file://' + overviewPageOutDir + 'releases.css" rel="stylesheet" type="text/css"/>' +
                        '<a href="' + overviewPageOutDir + 'appOverview.html"> Up </a>' +  
                        '</head>')

    #Outputs [version, release, PLDI2005 link, George link] in order of build date in table format
    overviewPage.write('<body>\n')
    overviewPage.write('<table border="1">\n')
    overviewPage.write('<tr><th> Version </th><th> Release </th><th> Predicate View </th><th> George View </th></tr>\n')
    
    for month in sorted(indexedBuilds, reverse=True):
        for day in sorted(indexedBuilds[month], reverse=True):
            distribution = indexedBuilds[month][day][0]
            version = indexedBuilds[month][day][1]
            release = indexedBuilds[month][day][2]
            pldiLink = '<img src="' + overviewPageOutDir + 'grayedFavicon.png"></img>'
            if distribution in os.listdir(overviewPageOutDir) and (appName+'-'+version+'-'+release) in os.listdir(overviewPageOutDir+distribution) and 'all_hl_corrected-exact-complete.xml' in os.listdir(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release):
                numPreds = countPreds(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release+'/all_hl_corrected-exact-complete.xml')
                if numPreds > 0:
                    pldiLink = ''+str(numPreds)+'&nbsp;&nbsp;<a href="' + overviewPageOutDir + distribution + '/' + appName+'-'+version+'-'+release + '/all_hl_corrected-exact-complete.xml"><img src="' + overviewPageOutDir + 'favicon.png"></img></a> &nbsp;&nbsp;'
            if distribution in os.listdir(overviewPageOutDir) and (appName+'-'+version+'-'+release) in os.listdir(overviewPageOutDir+distribution) and 'src' in os.listdir(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release) and 'index.html' in os.listdir(overviewPageOutDir+distribution+'/'+appName+'-'+version+'-'+release+'/src'):
                georgeLink = '<a href="' + overviewPageOutDir + distribution + '/' + appName+'-'+version+'-'+release + '/src/index.html"><img src="' + overviewPageOutDir + 'georgeicon.png"></img></a>'
            else:
                georgeLink = '<img src="' + overviewPageOutDir + 'grayedGeorgeicon.png"></img>'

            version_release = (version+'-'+release).replace('.','-')
            overviewPage.write('<tr id="' + appName + '-' + version_release + '">' +
                               '<td>' + version + '</td><td>' + release + '</td>' +
                               '<td align="center" height="40" valign="middle">' + pldiLink + '</td>' +
                               '<td align="center">' + georgeLink + '</td>' +
                               '</tr>\n')
            
    overviewPage.write('</table>')
    overviewPage.write('</body>')

    #Closes the document
    overviewPage.write('</html>')
    overviewPage.close()

    print "Wrote", overviewPageOutDir + appName + "/index.html"
    

######################################
#
# Creates the final HTML document with
# links to each application
#
######################################
def createAppOverview(indexedBuilds, overviewPageOutDir):

    #Opens the final html file for writing
    overviewPage = open(overviewPageOutDir+"appOverview.html", "w")

    #Generates some opening html tags
    overviewPage.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"' +
            '"http://www.w3.org/TR/html4/loose.dtd">' +
            '<html>')

    #Generates a header
    overviewPage.write('<head>' +
                        '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">' +
                        '<title>CBI Application Overview</title>' +
                        '<link href="file://' + overviewPageOutDir + 'releases.css" rel="stylesheet" type="text/css"/>' +
                        '</head>')

    #Outputs links to each application, generates index pages for each application
    overviewPage.write('<body>')
    for app in sorted(indexedBuilds):
        overviewPage.write('<ul>')
        overviewPage.write('<li><a href="' + app + '/index.html" id="' + app + '"><font size="4"><strong>' + app + '</strong></font></a></li>\n')
        createAppPage(indexedBuilds[app], overviewPageOutDir, app)
        overviewPage.write('</ul>')
    overviewPage.write('</body>')

    #Closes the document
    overviewPage.write('</html>')
    overviewPage.close()

    print "Wrote", overviewPageOutDir+"appOverview.html"


######################################
#
# Creates the final HTML document with
# links to each distribution
#
######################################
def createDistOverview(indexedBuilds, overviewPageOutDir):

    #Opens the final html file for writing
    overviewPage = open(overviewPageOutDir+"distOverview.html", "w")

    #Generates some opening html tags
    overviewPage.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"' +
            '"http://www.w3.org/TR/html4/loose.dtd">' +
            '<html>')

    #Generates a header
    overviewPage.write('<head>' +
                        '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">' +
                        '<title>CBI Distribution Overview</title>' +
                        '<link href="file://' + overviewPageOutDir + 'releases.css" rel="stylesheet" type="text/css"/>' +
                        '</head>')

    #Outputs links to each application, generates index pages for each application
    overviewPage.write('<body>')
    for dist in sorted(indexedBuilds):
        overviewPage.write('<ul>')
        overviewPage.write('<li><a href="' + dist + '/index.html" id="' + dist + '"><font size="4"><strong>' + dist + '</strong></font></a></li>\n')
        createDistPage(indexedBuilds[dist], overviewPageOutDir, dist)
        overviewPage.write('</ul>')
    overviewPage.write('</body>')

    #Closes the document
    overviewPage.write('</html>')
    overviewPage.close()

    print "Wrote", overviewPageOutDir+"distOverview.html"


######################################
#
# Generates css highlighting for each
# application release
#   - the background color where any release
#     is listed is set by its crash rate
#
######################################
def generateCSS(overviewPageOutDir, interestingBuilds):

    cssFile = open(overviewPageOutDir+"releases.css", "w")

    #Write highlighting color for each build based on crash rate
    for build in interestingBuilds:
        appName = build[0]
        version = build[2]
        release = build[3]
        version_release = (version+'-'+release).replace('.','-')
        idTag = appName+'-'+version_release
        crashRate = build[6]
        if crashRate > 0.05:
            color = "#ff8080"
        elif crashRate > 0.02:
            color = "#ffff80"
        else:
            color = "#a0ffa0"
        cssFile.write('#' + idTag + ' { background-color: ' + color + ' }\n')

    mostRecentBuilds = getMostRecentBuilds(interestingBuilds)

    #Write highlighting color for each app based on crash rate of most recent build
    for appName in mostRecentBuilds:
        crashRate = mostRecentBuilds[appName][0]
        if crashRate > 0.05:
            color = "#ff8080"
        elif crashRate > 0.02:
            color = "#ffff80"
        else:
            color = "#a0ffa0"
        cssFile.write('#' + appName + ' { background-color: ' + color + ' }\n')

    cssFile.close()


######################################
#
# Returns in list form the given indexed
# list of builds, flattened
#
######################################
def flatten(indexedBuilds):

    flattenedBuilds = []
    for month in indexedBuilds:
        for day in indexedBuilds[month]:
            for appName in indexedBuilds[month][day]:
                version = indexedBuilds[month][day][appName][0]
                release = indexedBuilds[month][day][appName][1]
                flattenedBuilds.append([appName, version, release, month, day])
                
    return flattenedBuilds


######################################
#
# Returns the list of the most recent builds for each app
#  -includes crash rate
#
######################################
def getMostRecentBuilds(interestingBuilds):

    mostRecentBuilds = {}
    for build in interestingBuilds:
        appName = build[0]
        distribution = build[1]
        version = build[2]
        release = build[3]
        month = build[4]
        day = build[5]
        crashRate = build[6]
        if appName not in mostRecentBuilds:
            mostRecentBuilds[appName] = [0.0, 0, 0, '', '', '']
        if month > mostRecentBuilds[appName][1]:
            mostRecentBuilds[appName] = [crashRate, month, day, distribution, version, release]
        elif month == mostRecentBuilds[appName][1]:
            if day > mostRecentBuilds[appName][2]:
                mostRecentBuilds[appName] = [crashRate, month, day, distribution, version, release]

    return mostRecentBuilds


######################################
#
# Returns the list of the most recent builds for each app
#  -assumes that the given list is narrowed to a single distribution
#
######################################
def getMostRecentDistBuilds(interestingBuilds):

    mostRecentBuilds = {}
    for build in interestingBuilds:
        appName = build[0]
        version = build[1]
        release = build[2]
        month = build[3]
        day = build[4]
        if appName not in mostRecentBuilds:
            mostRecentBuilds[appName] = [0, 0, '', '']
        if month > mostRecentBuilds[appName][0]:
            mostRecentBuilds[appName] = [month, day, version, release]
        elif month == mostRecentBuilds[appName][0]:
            if day > mostRecentBuilds[appName][1]:
                mostRecentBuilds[appName] = [month, day, version, release]

    return mostRecentBuilds


######################################
#
# Generates links for each summary page
# in given summary array, and returns them
# in the same array.
#
# Deprecated.
#
######################################
def generateLinks(summaryArray, analysisDir):

    for distribution in summaryArray:
        for release in summaryArray[distribution]:
            summaryArray[distribution][release][0] = '<a href="' + analysisDir + '/' + distribution + '/' + release + '/summary.xml">' + release + '</a>'

    return summaryArray


######################################
#
# Queries the database for "interesting builds"
#  - those builds with some failures to analyze
#
# Ouput list row is of the form:
#  appName, distribution, version, release, month(since year 0 A.D.), dayOfTheMonth
#
######################################
def getInterestingBuilds(queryname):
    db = PgSQL.connect(host='postgresql.cs.wisc.edu', port=49173, database='cbi')
    db.autocommit = False

    query = file(queryname).read()
    cursor = db.cursor()
    cursor.execute(query)

    while True:
        rows = cursor.fetchmany()
        if rows:
            for apName, version, release, distribution, year, month, day in rows:
                yield apName, distribution, version, release, 12 * year + month, day
        else:
            break

    db.rollback()
    db.close()

######################################
#
# Queries the database for the number of runs
# for each app release
#
# Ouput list row is of the form:
#  appName, distribution, version, release, numRuns
#
######################################
def getRuns(queryname):
    db = PgSQL.connect(host='postgresql.cs.wisc.edu', port=49173, database='cbi')
    db.autocommit = False

    query = file(queryname).read()
    cursor = db.cursor()
    cursor.execute(query)

    while True:
        rows = cursor.fetchmany()
        if rows:
            for apName, version, release, distribution, numRuns in rows:
                yield apName, distribution, version, release, numRuns
        else:
            break

    db.rollback()
    db.close()


######################################
#
# Appends each build in interestingBuilds
# with crash rate number
#
######################################
def appendCrashRate(interestingBuilds, buildListAll, buildListFails):

    #First index buildLists by appName => version => release
    allBuilds = {}
    for build in buildListAll:
        #top index is by app
        appName = build[0]
        if appName not in allBuilds:
            allBuilds[appName] = {}
        #second index is by version
        version = build[2]
        if version not in allBuilds[appName]:
            allBuilds[appName][version] = {}
        #third index is by release
        release = build[3]
        if release not in allBuilds[appName][version]:
            allBuilds[appName][version][release] = build[4]

    failBuilds = {}
    for build in buildListFails:
        #top index is by app
        appName = build[0]
        if appName not in failBuilds:
            failBuilds[appName] = {}
        #second index is by version
        version = build[2]
        if version not in failBuilds[appName]:
            failBuilds[appName][version] = {}
        #third index is by release
        release = build[3]
        if release not in failBuilds[appName][version]:
            failBuilds[appName][version][release] = build[4]

    #Now append crash rate to each build
    appendedBuildList = []
    for build in interestingBuilds:
        appName = build[0]
        version = build[2]
        release = build[3]
        crashRate = (failBuilds[appName][version][release]+0.0) / (allBuilds[appName][version][release]+0.0)
        appendedBuildList.append([build[0], build[1], build[2], build[3], build[4], build[5], crashRate])

    return appendedBuildList
        


######################################
#
# Indexes the given list by appName, then by date
#
# Ouput dictionary is of the form:
#  appName =>
#             month =>
#                      day =>
#                             distribution, version, release
#
######################################
def indexByAppname_Date(interestingBuilds):

    indexedBuilds = {}
    for build in interestingBuilds:
        #top index is by app
        appName = build[0]
        if appName not in indexedBuilds:
            indexedBuilds[appName] = {}

        #second index is by build month
        month = build[4]
        if month not in indexedBuilds[appName]:
            indexedBuilds[appName][month] = {}

        #third index is by day of the month
        day = build[5]
        if day not in indexedBuilds[appName][month]:
            indexedBuilds[appName][month][day] = [build[1], build[2], build[3]]

    return indexedBuilds


######################################
#
# Indexes the given list by distribution, then by date, then by appName
#
# Ouput dictionary is of the form:
#  distribution =>
#                  month =>
#                           day =>
#                                  appName =>
#                                             version, release
#
######################################
def indexByDistribution_Date(interestingBuilds):

    indexedBuilds = {}
    for build in interestingBuilds:
        #top index is by distribution
        distribution = build[1]
        if distribution not in indexedBuilds:
            indexedBuilds[distribution] = {}

        #second index is by build month
        month = build[4]
        if month not in indexedBuilds[distribution]:
            indexedBuilds[distribution][month] = {}

        #third index is by day of the month
        day = build[5]
        if day not in indexedBuilds[distribution][month]:
            indexedBuilds[distribution][month][day] = {}

        #fourth index is by appName
        appName = build[0]
        if appName not in indexedBuilds[distribution][month][day]:
            indexedBuilds[distribution][month][day][appName] = [build[2], build[3]]

    return indexedBuilds


######################################
#
# Indexes the given list by appName
#
# Ouput dictionary is of the form:
#                  appName =>
#                             version, release
#
######################################
def indexByApp(interestingBuilds):

    indexedBuilds = {}
    for build in interestingBuilds:

        #second index is by appName
        appName = build[0]
        if appName not in indexedBuilds[distribution]:
            version = build[1]
            release = build[2]
            indexedBuilds[appName] = interestingBuilds

        #third index is by day of the month
        day = build[5]
        if day not in indexedBuilds[distribution][month]:
            indexedBuilds[distribution][month][day] = {}

        #fourth index is by appName
        appName = build[0]
        if appName not in indexedBuilds[distribution][month][day]:
            indexedBuilds[distribution][month][day][appName] = [build[2], build[3]]

    return indexedBuilds
        


######################################
#
# Gets all the summary pages in the given
# directory, and returns those in an array
# indexed by build distribution, release.
#
# Deprecated.
#
######################################
def getSuccessfulSummaryPages(analysisDir, undo, backup):

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
                #Undo George highlighting
                if undo == True:
                    undoHighlight(analysisPath)

                #Update links in predicate table to make what they link to more readable
                #processPredicateLinks(analysisPath)
                
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
# Deprecated.
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
# Creates a copy of all the src html
# files and stores them in georgesrc/
#
# Deprecated.
#
######################################
def copyToGeorge(resultsDir):
    if not resultsDir.endswith("/"):
        resultsDir += "/"
    if 'georgesrc' not in os.listdir(resultsDir):
        commands.getoutput('mkdir ' + resultsDir + 'georgesrc')
    copyToDir = resultsDir + "georgesrc/"
    copyFromDir = resultsDir + "src/"
    commands.getoutput("../george/utils/copy_dir_files " + copyFromDir + " " + copyToDir) 
    

def main():

    ##TODO
    # 0. Complete distribution view
    # 0.1. Change SQL to run on every build, not just interesting ones
    # 1. Copy all things of interest to given output directory (summary.xml, supporting xmls, debug dir, etc.)
    # 2. Overview should go at top of that --- provide option to not copy everything (just place overview on top)

    if len(sys.argv) != 3 and len(sys.argv) != 4:
        print "Usage: create_overview_page.py analysisDir overviewPageOutDir [-u -b]"
        print "Where -u indicates that George highlighting should be undone."
        print "Where -b indicates that a backup of src html should be made."
        return

    analysisDir = sys.argv[1]
    if not analysisDir.endswith('/'):
        analysisDir += '/'
    overviewPageOutDir = sys.argv[2]
    if not overviewPageOutDir.endswith('/'):
        overviewPageOutDir += '/'
    undo = False
    backup = False
    if len(sys.argv) == 4:
        option = sys.argv[3]
        if option == "-u":
            undo = True
        elif option == "-b":
            backup = True

    interestingBuilds = list(getInterestingBuilds("interesting_builds.sql"))
    buildListAll = list(getRuns("runs.sql"))
    buildListFails = list(getRuns("crashes.sql"))
    interestingBuilds = appendCrashRate(interestingBuilds, buildListAll, buildListFails)

    generateCSS(overviewPageOutDir, interestingBuilds)
    
    appName_indexedBuilds = indexByAppname_Date(interestingBuilds)
    createAppOverview(appName_indexedBuilds, overviewPageOutDir)
    
    distribution_indexedBuilds = indexByDistribution_Date(interestingBuilds)
    createDistOverview(distribution_indexedBuilds, overviewPageOutDir)

    #summaryArray = getSuccessfulSummaryPages(analysisDir, undo, backup)
    #linksArray = generateLinks(summaryArray, analysisDir)
    #createOverviewPage(linksArray, overviewPageOutDir)

    if debug > 1:
        commands.getoutput("firefox " + overviewPageOutDir + "distOverview.html")


if __name__ == '__main__':
    main()

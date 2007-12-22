#!/s/std/bin/python -O

######################################
#
# Authors: Jason Fletchall, Lydia Sidrak, 11/2007
#
# This script gets a count of the number of times 
# each instrumentation point is sampled in the aggregated
# reports from the given feedback report directory.
# It combines that information with filename + linenumber
# in source code, finally writing the list of coverage,
# crashiness, and crash-impact statistics, calculated
# from the sample data, to a file that is readable
# by the visualization tool

# Usage: calculate-stats.py reportsDir sitesDir vizOutDir srcHtmlDir distributionName releaseName
#
######################################

from copy import deepcopy
import sys
import os
import string
import commands

debug = 0
makeFiles = 0 #make sites and report file flag

######################################
#
# Returns the list of the amount of times each instrumentation point was sampled
# in a single report.
#
#  Megarray is a nested array that stores totals per instrumentation schemes as so:
# {unit_id1} => {scheme} =>
#	            {tot1, tot2...totn},
#               {scheme2} =>
#  	            {tot1, totk}
# where n is the number of lines listed with a <sites ....> </site> tags 
#
######################################
def addInstrumentationPointCoverage(reportsFile, sitesArray, megaArray):

    if debug > 1:
        print "addInstrumentationPointCoverage() aka aggregate reportArray "
        
    # Opens the report file for reading
    infile = open(reportsFile, "r")

    # Makes a list of each line in the report file
    lines = infile.readlines()    
    count = 1
    
	# Build multidimensional array with reports
    #megaArray = {}
    lineNums = []
    sanity_unit_ids = []
    dup_unit_ids = []
    curScheme = ''
    filename = '' 
    unit_id = ''
    newSums = []
    newTot = 0
    numElements = 0
    sum = 0
    i = 0 
    for line in lines:
        #print line
        if line.startswith("</samples>"):  
            if unit_id in sitesArray and len(lineNums) == len(sitesArray[unit_id][curScheme]):
                if curScheme in megaArray[unit_id]:#we must condense sums
                    numElements = len(megaArray[unit_id][curScheme])
                    if (numElements > 0):
                        i = 0   
                        while i < numElements:
                            newTot = int(megaArray[unit_id][curScheme][i])+int(lineNums[i]) 
                            newSums.append(newTot)
                            i = i+1
                        megaArray[unit_id][curScheme] = newSums
                        newSums = []
                else:                                 
                    megaArray[unit_id][curScheme] = lineNums 
        elif line.startswith("<samples "):
            #Abort if this samples tag does not have unit and scheme
            if line.find("unit=") == -1 or line.find("scheme=") == -1:
                print "Offensive line in report:\n", line
                return -1
            
            lineNums = [] 

            # extract unit_id
            splitLine = line.split(' unit="') 
            splitLine = splitLine[1].split('"')
            unit_id = splitLine[0]

            if len(splitLine) < 3:
                print "Offensive line in report:\n", line
                return -1
            
            # sanity checking
            #if not unit_id in sanity_unit_ids:
            #    sanity_unit_ids.append(unit_id)
            #else:
            #    dup_unit_ids.append(unit_id)

            #extract scheme
            curScheme = splitLine[2]

            if unit_id in megaArray: #add scheme subarray if there's already a unit_id in megaArray, and unit_id in sitesArray
                #print unit_id +" already in megaArray"
                if curScheme in sitesArray[unit_id] and not curScheme in megaArray[unit_id]:
                    #print "  adding [" +unit_id+"]["+curScheme +"]"  
                    megaArray[unit_id][curScheme] = [] 
            elif unit_id in sitesArray:# add unit_id key to megaArray
                megaArray[unit_id] = {} 
        else:
            #Sometimes we get messed up reports with tags floating on data lines.
            #  Just return with what we have if that is the case.
            if line.find("<") != -1:
                print "Offensive line in report:\n", line
                return -1
            splitLine = line.split()
            for num in splitLine:
                try:
                    value = int(num)
                except ValueError:
                    print "Offensive line in report:\n", line
                    return -1
                sum = value + sum   
            lineNums.append(sum)
            sum = 0 
    infile.close()
    key = ''

    if (debug > 1):
        print "\n###### REPORT FILE STATS ######"
        print "distinct unit_ids: ",len(sanity_unit_ids)
        print "duplicate unit_ids: ",len(dup_unit_ids)
        print "actual unit_ids:",len(megaArray)
        
    return megaArray


######################################
#
# Returns the list of instrumented source code lines from sitesFile
#
# Returned array is in the form: 
# sitesArray =>
#            (unit_id1) => (scheme1) => "filepath:lineno1"
#                                    =>  "filepath:lineno2"
#                          (scheme2) => "filepath:lineno1"
#            (unit_id2) => ....
#
######################################
def getSourceInstrumentationList(sitesFile):

    # Opens the given file for reading
    infile = open(sitesFile, "r")

    # Makes a list of each line in the report file
    lines = infile.readlines()    
    count = 1
    
	# Build multidimensional array with schemes and instrumented line numbers
    megaArray = {}
    lineNums = []
    sanity_unit_ids = []
    dup_unit_ids = []
    curScheme = ''
    filename = ''  
    unit_id = ''
    key = ''
    print "in getSource... filename", sitesFile
    for line in lines:
        if line.startswith("</sites>"):
            megaArray[unit_id][curScheme] = lineNums
        elif line.startswith("<sites "):
            lineNums = [] 
            # extract unit
            splitLine = line.split(' unit="') 
            splitLine = splitLine[1].split('"')
            unit_id = splitLine[0]

            if not unit_id in sanity_unit_ids:
                sanity_unit_ids.append(unit_id)
            else:
                dup_unit_ids.append(unit_id)  

            #extract scheme
            curScheme = splitLine[2]
            if unit_id in megaArray:
                if not curScheme in megaArray[unit_id]:
                    megaArray[unit_id][curScheme] = [] 
            else:
                megaArray[unit_id] = {}
        elif not line.startswith("#####"):
            splitLine = line.split()
            if len(splitLine) > 1:
                filename = splitLine[0]
                lineNum =  splitLine[1]
                if debug > 1:
                    print filename + ":" + lineNum
                lineNums.append(filename + ":" + lineNum)  
                count = count + 1
    infile.close()

    if debug > 1:
        print "###### SITES FILE STATS ######"
        print "distinct unit_ids: ",len(sanity_unit_ids)
        print "duplicate unit_ids: ",len(dup_unit_ids)
        print "actual unit_ids:",len(megaArray)

    #for key in megaArray:
    #    print "\n"+key
    #    for key2 in megaArray[key]:
    #        print key2, "has ", len(megaArray[key][key2]),"elements"
            #for each in megaArray[key][key2]:
                #print each
                
    return megaArray  
 
#############################################################################
#
# Combine all sites data into one MEGA.site file
#
#############################################################################
def generateSitesFile(mySitesDir):
   
    print "Generating MEGA sites file..."
    # Opens a MEGA.sites file for writing
    outfile = open(mySitesDir + "/MEGA.sites", "w")

    if not os.path.isdir(mySitesDir): 
       print "Invalid sites directory path:", mySitesDir,"\n"
       return

    for root, dirs, files in os.walk(mySitesDir):
        for file in files:
            if not 'MEGA.sites' in file and ".sites" in file:
                outfile.write("###################"+root+"/"+file+"########################\n") 
                inSitesFile = open(root+"/"+file)
                print "Including", root+"/"+file, "." 
                lines = inSitesFile.readlines()
                for line in lines:
                    #print line
                    outfile.write(str(line))
    outfile.close


#####################################
#
# Returns in array form the coverage information for all instrumentation
# points aggregated across all reports.
#
#####################################
def getAggregateCoverage(reportsDir, sitesArray):

    print "Aggregating reports in", reportsDir, "..."

    # Adds coverage data on instrumentation points for each report in the given directory
    numReports = 0
    fullCoverageArray = {}
    for reportPath, dirs, reports in os.walk(reportsDir):
        for report in reports:
            if (report == "reports"):
                numReports += 1
                returnValue = addInstrumentationPointCoverage(reportPath+"/"+report, sitesArray, fullCoverageArray)
                if returnValue == -1:
                    print "Malformed report -", numReports, "- Throwing it out."
                else:
                    fullCoverageArray = returnValue
                    print "Added report", numReports

    if debug > 0:
        print "Found", numReports, "reports total in", reportsDir
                        
    return fullCoverageArray


#####################################
#
# Returns in array form the coverage information for all instrumentation
# points aggregated across all reports associated with failure.
#
#####################################
def getAggregateFailures(reportsDir, sitesArray):

    print "Aggregating failure reports in", reportsDir, "..."

    # Walks the given directory looking for reports files.  For each reports file,
    # looks in the corresponding label file and only adds in the report if label
    # indicates failure.
    numReports = 0
    fullCoverageArray = {}
    for reportPath, dirs, files in os.walk(reportsDir):
        for thisFile in files:
            if (thisFile == "reports"):
                labelFile = open(reportPath+"/"+"label", "r")
                label = labelFile.readline()
                if (label == "failure\n"):
                    numReports += 1
                    returnValue = addInstrumentationPointCoverage(reportPath+"/"+thisFile, sitesArray, fullCoverageArray)
                    if returnValue == -1:
                        print "Malformed report -", numReports, "- Throwing it out."
                    else:
                        fullCoverageArray = returnValue
                        print "Added failure report", numReports
                labelFile.close()

    if debug > 0:
        print "Found", numReports, "failure reports total in", reportsDir
                        
    return fullCoverageArray


##############################################################################
#
# Combine all reports data into one MEGA.report file
#
# No longer used
#
#############################################################################
def generateReportsFile(reportsFilename, myReportsDir):

    print "generateReportsFile()"
    # Parse through reports and build a mega reports file
    outfile = open(reportsFilename, "w")
    if not os.path.isdir(myReportsDir):
        print "invalid path\n"

    for root, dirs, files in os.walk(myReportsDir):
        for file in files:
            if 'report' in file:
                print root,dir,file
                for line in open(root+"/"+file).readlines():
                        #print "w:",line
                    outfile.write(str(line))
    outfile.close


##############################################################################
#
# Appends the given file-line-score list to the .viz file
#
############################################################################
def appendDataToVizFile(fileLineScoreArray, srcHtmlDir, vizOutFile, release):
    
    for srcFileName in fileLineScoreArray:
        for lineNo in fileLineScoreArray[srcFileName]:
            app_versionbits = release.rsplit("-", 1)
            if len(app_versionbits) == 2:
                #split on applicationName-version
                srcFilePath = ""
                path_bits = srcFileName.rsplit(app_versionbits[0], 1)
                if len(path_bits) == 2:
                    srcFilePath = app_versionbits[0] + path_bits[1]
                else:
                    srcFilePath = srcFileName
                html_dir = srcHtmlDir + "/" + srcFilePath + ".html"
                html_dir,html_file =  html_dir.rsplit("/",1)
            
                thisLine = fileLineScoreArray[srcFileName][lineNo][0] + "," + html_dir+"/" + "," + html_file + "," + str(lineNo) + "," + str(fileLineScoreArray[srcFileName][lineNo][1]) + "\n"
                vizOutFile.write(thisLine)
            

##############################################################################
#
# Changes scores in the given array of scores into percentile rankings
#
#############################################################################
def percentilize(fileLineScoreArray):
    
    #Calculates raw scores for each line while keeping a sorted list of all raw scores
    rawScoreList = []
    for srcFileName in fileLineScoreArray:
        for lineNo in fileLineScoreArray[srcFileName]:
            rawScore = (fileLineScoreArray[srcFileName][lineNo][1] + 0.0) / (fileLineScoreArray[srcFileName][lineNo][2] + 0.0)
            fileLineScoreArray[srcFileName][lineNo] = [fileLineScoreArray[srcFileName][lineNo][0], rawScore]
            rawScoreList.append(rawScore)
    rawScoreList.sort()


    #Now re-score all the lines based on percentile
    #scoreIndexedArray = {}
    for srcFileName in fileLineScoreArray:
        for lineNo in fileLineScoreArray[srcFileName]:
            percentileScore = 0

            #If raw score is 0, set percentile score to 0.  Otherwise, calculate percentile.
            if fileLineScoreArray[srcFileName][lineNo][1] > 0.0:
                #do a binary search of rawScoreList to find this line's score --
                #  the index in the array of the score indicates its percentile
                top = len(rawScoreList) - 1
                bottom = 0
                i = top / 2
                while(1):
                    if fileLineScoreArray[srcFileName][lineNo][1] == rawScoreList[i]:
                        break
                    elif fileLineScoreArray[srcFileName][lineNo][1] > rawScoreList[i]:
                        bottom = i + 1
                    else:
                        top = i - 1
                    i = (bottom + top) / 2
                percentileScore = (i + 0.0) / (len(rawScoreList) - 1.0) * 100
                
            fileLineScoreArray[srcFileName][lineNo][1] = int(percentileScore)

            if debug > 1:
                print i, ",", len(rawScoreList), ",", percentileScore

    return fileLineScoreArray


#############################################################################
#
# Returns a list of each line of source code mapped to a raw statistical score
#
# List is of the form:
# {srcHtmlFileName} =>
#                 {lineNo} =>
#	                     [statName, totalCoverage, numPoints]
#
#############################################################################
def combineDataWithSites(sitesArray, megaArray, statName):

    # We have an associative array of site and report numbers
    # Export all report data for each unit_id and scheme combination
    # Note, we may have more site information that actual data (instrumentation funky)
    
    fileLineScoreArray = {}
        
    res = ''
    for unit_id in sitesArray:
        for scheme in sitesArray[unit_id]:
            # if unit_id and schemes are in both sitesArray and megaArrays    
            if unit_id in sitesArray and scheme in sitesArray[unit_id] and unit_id in megaArray and scheme in megaArray[unit_id]:
                #print "  scheme:",scheme," has ", len(sitesArray[unit_id][scheme])," siteElements and ",len(megaArray[unit_id][scheme]), " reportElements"
                i=0
                html_dir = ''
                if len(megaArray[unit_id][scheme]):
                    for bit in sitesArray[unit_id][scheme]:
                        srcFileName,lineNo = bit.split(":")
                        if i < len(megaArray[unit_id][scheme]):
                            #If src file is already mentioned in the array, start a new value only if the line number is unique.
                            #  if line number is not unique, sum this data with previous data for this line
                            if srcFileName in fileLineScoreArray:
                                if not lineNo in fileLineScoreArray[srcFileName]:
                                    fileLineScoreArray[srcFileName][lineNo] = [statName, megaArray[unit_id][scheme][i], 1]
                                else:
                                    fileLineScoreArray[srcFileName][lineNo][1] += megaArray[unit_id][scheme][i]
                                    fileLineScoreArray[srcFileName][lineNo][2] += 1
                            else:
                                fileLineScoreArray[srcFileName] = {}
                            i = i+1 
                #else:
                #    print "no report data for [",unit_id,"][",scheme,"]"
                
    return fileLineScoreArray


#############################################################################
#
# Returns a list of each line of source code mapped to a raw crashiness score.
# Mirrors the output of combineDataWithSites() to keep consistent for other functions.
#
# List is of the form:
# {srcHtmlFileName} =>
#                 {lineNo} =>
#	                     ["crashiness", crashinessScore, 1]
#
#############################################################################
def combineCoverageWithCrashes(coverageArray, crashimpactArray):

    crashinessArray = {}

    for srcFileName in coverageArray:
        for lineNo in coverageArray[srcFileName]:
            if srcFileName in crashimpactArray and lineNo in crashimpactArray[srcFileName]:
                coverageScore = (coverageArray[srcFileName][lineNo][1]+0.0) / (coverageArray[srcFileName][lineNo][2]+0.0)
                crashimpactScore = (crashimpactArray[srcFileName][lineNo][1]+0.0) / (crashimpactArray[srcFileName][lineNo][2]+0.0)
                
                #If a line has never been covered, it automatically has no crashiness
                if coverageScore > 0:
                    crashinessScore = crashimpactScore / coverageScore
                else:
                    crashinessScore = 0
                    
                if srcFileName in crashinessArray:
                    crashinessArray[srcFileName][lineNo] = ["crashiness", crashinessScore, 1]
                else:
                    crashinessArray[srcFileName] = {}
                    crashinessArray[srcFileName][lineNo] = ["crashiness", crashinessScore, 1]
                     
            #Coverage and crashimpact data should line up, but just ignore the mismatches if they exist
            else:
                if debug > 0:
                    print "Line in coverage not found in crashimpact:", srcFileName, ":", lineNo

    return crashinessArray


##############################################################################
#
# Calculates code coverage data and appends it to the given .viz file
#
#############################################################################
    
def calculateCoverage(sitesArray, reportsDir, srcHtmlDir, vizOutFile, release): 
    print "Calculating coverage data..."

    #Gets all the coverage data from the reports
    megaArray = getAggregateCoverage(reportsDir, sitesArray)
   
    if debug > 0:
        print len(megaArray), " reportElements vs ", len(sitesArray), " siteElements \n"

    #Combines the two arrays to create a file-line-score array
    fileLineScoreArray = combineDataWithSites(sitesArray, megaArray, "coverage")

    #Now change the scores to percentiles
    percentilizedFileLineScoreArray = deepcopy(fileLineScoreArray)
    percentilizedFileLineScoreArray = percentilize(percentilizedFileLineScoreArray)

    #Finally, dump all this to the .viz file
    appendDataToVizFile(percentilizedFileLineScoreArray, srcHtmlDir, vizOutFile, release)

    #Return the raw scores so we can calculate crashiness later
    return fileLineScoreArray


##############################################################################
#
# Calculates code coverage data for failed runs and appends it to the given .viz file
#
#############################################################################
    
def calculateCrashimpact(sitesArray, reportsDir, srcHtmlDir, vizOutFile, release): 
    print "Calculating failure data..."

    #Gets all the coverage data from the reports
    megaArray = getAggregateFailures(reportsDir, sitesArray)
   
    if debug > 0:
        print len(megaArray), " reportElements vs ", len(sitesArray), " siteElements \n"

    #Combines the two arrays to create a file-line-score array
    fileLineScoreArray = combineDataWithSites(sitesArray, megaArray, "crash-impact")

    #Now change the scores to percentiles
    percentilizedFileLineScoreArray = deepcopy(fileLineScoreArray)
    percentilizedFileLineScoreArray = percentilize(percentilizedFileLineScoreArray)

    #Finally, dump all this to the .viz file
    appendDataToVizFile(percentilizedFileLineScoreArray, srcHtmlDir, vizOutFile, release)

    #Return the raw scores so that we can calculate crashiness later
    return fileLineScoreArray


##############################################################################
#
# Calculates code coverage data for failed runs and appends it to the given .viz file
#
#############################################################################
    
def calculateCrashiness(coverageArray, crashimpactArray, srcHtmlDir, vizOutFile, release): 
    print "Calculating crashiness data..."
   
    #Combines the two arrays to create a file-line-score array
    fileLineScoreArray = combineCoverageWithCrashes(coverageArray, crashimpactArray)

    #Now change the scores to percentiles
    percentilizedFileLineScoreArray = percentilize(fileLineScoreArray)

    #Finally, dump all this to the .viz file
    appendDataToVizFile(percentilizedFileLineScoreArray, srcHtmlDir, vizOutFile, release)



######################################
#
# Calls functions to read in data about instrumentation points, 
#   then the list of sites, then puts them together
# 	and writes to a .viz file
######################################
def main():
    
    if (len(sys.argv) != 7):
        print "Usage: calculate-statistics.py reportsDir sitesDir vizOutDir srcHtmlDir distributionName releaseName"

    #Gets directories for input and output
    reportsDir = sys.argv[1]
    sitesDir = sys.argv[2]
    vizOutDir = sys.argv[3]
    srcHtmlDir = sys.argv[4]
    distribution = sys.argv[5]
    release = sys.argv[6]

    #Concatenates all the sites files into a single file
    generateSitesFile(sitesDir)
    #Gets an array containing all the sites information
    sitesArray = getSourceInstrumentationList(sitesDir+"/MEGA.sites")

    #Opens the final .viz file that will contain the data for the visualizer
    vizOutFileName = vizOutDir + "/" + distribution + "___" + release + ".viz"
    try:
        vizOutFile = open(vizOutFileName, "w")
    except IOError:
        commands.getoutput("mkdir -p " + vizOutDir)
        vizOutFile = open(vizOutFileName, "w")

    #Calculates all the statistics
    coverageArray = calculateCoverage(sitesArray, reportsDir, srcHtmlDir, vizOutFile, release)
    crashimpactArray = calculateCrashimpact(sitesArray, reportsDir, srcHtmlDir, vizOutFile, release)
    calculateCrashiness(coverageArray, crashimpactArray, srcHtmlDir, vizOutFile, release)

    vizOutFile.close()

    #Deletes the mega sites file
    #removeMegaSitesFile(sitesDir)

    return vizOutFileName


if __name__ == '__main__':
    main()

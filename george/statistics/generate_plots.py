#!/s/std/bin/python -O

######################################
#
# Authors: Jason Fletchall, 12/2007
#
# This script generates a plot of the crashiest
# files in a build based on data from the given
# .viz file.  The plot is placed in the given
# location with a .png extension.
#
# Usage: generate_plots.py vizFile outputFile
#
######################################


import locale
import commands

import sys
sys.path.append('/unsup/PyChart-1.39/lib/python2.4/site-packages')
from pychart import *

from os.path import exists
from os.path import dirname
top = dirname(__file__)


######################################
#
# Returns a dictionary of all the data
# from the given .viz file, separated
# by statistic type.
#
######################################
def readStatisticalData(vizFile):
    #Get all lines from viz file
    infile = open(vizFile, "r")
    dataLines = infile.readlines()

    # Gets all files into an array - each file retains only its highest score
    unorderedFiles = {}
    for line in dataLines:
        statType, html_path, html_file, lineNo, score = line.split(",")
        if statType not in unorderedFiles:
            unorderedFiles[statType] = {}
        if html_file not in unorderedFiles[statType]:
            unorderedFiles[statType][html_file] = int(score)
        elif int(score) > unorderedFiles[statType][html_file]:
            unorderedFiles[statType][html_file] = int(score)

    # Organizes files by their crashiness score
    orderedList = {}
    for stat in unorderedFiles:
        orderedList[stat] = {}
        for fileName in unorderedFiles[stat]:
            if fileName in unorderedFiles['crashiness']:
                crashinessScore = unorderedFiles['crashiness'][fileName]
                score = unorderedFiles[stat][fileName]
                if crashinessScore > 94:
                    srcFileName = fileName.rsplit(".html", 1)[0]
                    if crashinessScore not in orderedList[stat]:
                        orderedList[stat][crashinessScore] = [(srcFileName, score)]
                    else:
                        orderedList[stat][crashinessScore].append((srcFileName, score))

    # Finally, organizes into way that PyChart can understand
    dataList = {}
    for stat in orderedList:
        if stat not in dataList:
            dataList[stat] = []
        for score in sorted(orderedList[stat], reverse=True):
            for pair in orderedList[stat][score]:
                dataList[stat].append(pair)

    return dataList


######################################
#
# Generates a bar plot of the given statistic
# and adds it to the given area.
#
######################################
def generatePlot(ar, dataList, statistic):
    data = dataList[statistic]
    label = statistic
    
    clusterVal = 0
    if statistic == "crash-impact":
        clusterVal = 1
    elif statistic == "crashiness":
        clusterVal = 2

    
    barWidth = 2 + (50 / len(dataList['crashiness']))
    bar = bar_plot.T(label=label, data=data, width=barWidth, fill_style=selectFill(statistic), line_style=selectLine(statistic), cluster=(clusterVal,3))
    ar.add_plot(bar)


#Rotates x-axis values by 90 degrees
def format_x(value):
    return '/a90{}%s' % (value)

def format_y(count):
    return locale.format('%d', count, grouping=True)

######################################
#
# Chooses a fill color based on stat type
#
######################################
def selectFill(statistic):
    if statistic == "coverage":
        return fill_style.Diag(bgcolor=color.black, line_style=selectLine(statistic))
    elif statistic == "crash-impact":
        return fill_style.Diag(bgcolor=color.black, line_style=selectLine(statistic))
    else:
        return fill_style.Diag(bgcolor=color.black, line_style=selectLine(statistic))

######################################
#
# Chooses a border line color based on stat type
#
######################################
def selectLine(statistic):
    if statistic == "coverage":
        return line_style.T(color=color.green)
    elif statistic == "crash-impact":
        return line_style.T(color=color.cyan)
    else:
        return line_style.T(color=color.yellow)


def main():

    if len(sys.argv) != 3:
        print "Usage: generate_plots.py vizFile outputFile"
        return

    vizFile = sys.argv[1]
    outputFile = sys.argv[2]

    chartOutputFile = outputFile + ".pdf"
    convertedFile = outputFile + ".png"
    
    locale.setlocale(locale.LC_ALL, '')

    print "Reading visualization data..."
    dataList = readStatisticalData(vizFile)

    #If we have no crashiness data, there isn't anything to plot
    if len(dataList['crashiness']) > 0:
        #Sets up the plot area
        x_axis = axis.X(format=format_x, label='Source File Name')
        y_axis = axis.Y(format=format_y, label='Percentile Score')
        ar = area.T(x_coord=category_coord.T(dataList['crashiness'],0), x_axis=x_axis,
                y_axis=y_axis, size=(940, 360), y_range=(90, 100), bg_style=fill_style.Plain(bgcolor=color.black))
        
        print 'Generating plots...'
        generatePlot(ar, dataList, "coverage")
        generatePlot(ar, dataList, "crash-impact")
        generatePlot(ar, dataList, "crashiness")

        #Writes the plots to a .pdf file
        can = canvas.init(chartOutputFile)
        ar.draw(can)
        can.close()

        #PyChart draws .png's badly, so we first create a .pdf then convert it ourselves
        print "Converting to png format..."
        commands.getoutput("convert " + chartOutputFile + " " + convertedFile)
        print "Generated", convertedFile
    
        commands.getoutput("rm -f " + chartOutputFile)
        
    else:
        print "No plot generated."


if __name__ == '__main__':
    main()

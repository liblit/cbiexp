#!/s/std/bin/python -O 

######################################
#
# Authors Lydia Sidrak, Jason Fletchall, 12/2007
#
# This script contains a function to make an overview
# page for an application-release.
#
######################################

import commands
import os
import sys
sys.path.append("../statistics")
import generate_plots

debug = 2

######################################
#
# Returns an array of file names sorted
# by score for each statistic.  Scores
# and file names are found on each line
# in the given list.
#
######################################
def orderFiles(lines):

    # Gets all files into an array - each file retains only its highest score
    unorderedFiles = {}
    for line in lines:
        statType, html_path, html_file, lineNo, score = line.split(",")
        if statType not in unorderedFiles:
            unorderedFiles[statType] = {}
        if html_file not in unorderedFiles[statType]:
            unorderedFiles[statType][html_file] = [html_path, int(score)]
        elif int(score) > unorderedFiles[statType][html_file][1]:
            unorderedFiles[statType][html_file][1] = int(score)

    #print unorderedFiles['crash-impact']
    # Organizes files by their score
    orderedList = {}
    for stat in unorderedFiles:
        orderedList[stat] = {}
        for fileName in unorderedFiles[stat]:
            score = unorderedFiles[stat][fileName][1]
            if score not in orderedList[stat]:
                orderedList[stat][score] = [[fileName, unorderedFiles[stat][fileName][0]]]
            else:
                orderedList[stat][score].append([fileName, unorderedFiles[stat][fileName][0]])

    return orderedList
 
    
######################################
#
# Finds the .viz file in the given vizRoot directory
# and create an overview html page based on its data.
#
######################################
def make_menu(vizRoot, srcHtmlDir, distribution, release):


    # Opens the given file for reading
    for path, dirs, fileNames in os.walk(vizRoot):
        for fileName in fileNames:
            if fileName.endswith(release+".viz"):
                vizFileName = path+"/"+fileName

    # Makes a list of each line in the report file
    infile = open(vizFileName, "r")
    lines = infile.readlines()

    # Generates a plot of the statistics
    sys.argv = ["junk", vizFileName, srcHtmlDir + "/stats_plot"]
    generate_plots.main()

    # Orders the referenced files found in the .viz file by statType, score
    orderedFiles = orderFiles(lines)

    outfile = open(srcHtmlDir+"/index.html","w")

    size = len(lines) # temporary work around to divy up results
    first_column = '<tr><td><h2>Coverage</h2></td></tr>'
    second_column = '<tr><td><h2>Crash-Impact</h2></td></tr>'
    third_column = '<tr><td><h2>Crashiness</h2></td></tr>'
    fourth_column = '<tr><td><h2>No Coverage</h2></td></tr>'
    fifth_column = '<tr><td><h2>No Crashes</h2></td></tr>'
    for stat in orderedFiles:
        for score in sorted(orderedFiles[stat], reverse=True):
            if score > 0:
                for htmlFile in orderedFiles[stat][score]:
                    # Link form is <a href="htmlFileName" title="Highest stat: score">srcFileName</a><br />
                    link = '<a id="'+stat+str(score)+'" href="'+htmlFile[1]+htmlFile[0]+'" title="Highest '+stat+': '+str(score)+'">'+htmlFile[0].rsplit(".",1)[0]+'</a><br />'
        
                    if stat == 'coverage': #first column
                        first_column = first_column+"<tr><td>"+link+"</td></tr>\n"
                    elif stat == 'crash-impact':   #second column
                        second_column = second_column+"<tr><td>"+link+"</td></tr>\n"            
                    else:    #third column
                        third_column = third_column+"<tr><td>"+link+"</td></tr>\n"
            
    #Files never covered
    for htmlFile in orderedFiles['coverage'][0]:
        # Link form is <a href="htmlFileName">srcFileName</a><br />
        link = '<a id="'+'coverage'+str(0)+'" href="'+htmlFile[1]+htmlFile[0]+'">'+htmlFile[0].rsplit(".",1)[0]+'</a><br />'
        fourth_column =fourth_column+"<tr><td>"+link+"</td></tr>\n"
    #Files covered, but no crashes
    for htmlFile in orderedFiles['crash-impact'][0]:
        if htmlFile not in orderedFiles['coverage'][0]:
            # Link form is <a href="htmlFileName">srcFileName</a><br />
            link = '<a id="'+'crash-impact'+str(0)+'" href="'+htmlFile[1]+htmlFile[0]+'">'+htmlFile[0].rsplit(".",1)[0]+'</a><br />'
            fifth_column =fifth_column+"<tr><td>"+link+"</td></tr>\n"


    # Make table of tables
    # Todo: Prettify this
    header = '<html>\n<head> \n<title> ' + release + ' </title>\n<link type="text/css" rel="stylesheet"  href="menu.css"/>\n <link type="text/css" rel="stylesheet"  href="maintable.css"/>\n </head>\n<body>\n' 
    plot_image = '<div align="left">\n<img src="stats_plot.png" title="This plot shows the crashiest files in this release"></img>\n</div>\n'
    first_column = '\n<table cellspacing="2" cellpadding="1" border="1">'+first_column+'</table>\n'
    second_column = '<table cellspacing="2" cellpadding="1" border="1">'+second_column+'</table>\n'
    third_column = '<table cellspacing="2" cellpadding="1" border="1">'+third_column+'</table>\n'
    fourth_column = '<table cellspacing="2" cellpadding="1" border="1">'+fourth_column+'</table>\n'
    fifth_column = '<table cellspacing="2" cellpadding="1" border="1">'+fifth_column+'</table>\n'
    html_file = header+plot_image+'<div id="main">\n <table align=\"left\"> <tr><td valign="top">'+first_column+'</td><td valign="top">'+second_column+'</td><td valign="top">'+third_column+'</td>\n <td valign="top">'+fourth_column+'</td><td valign="top">'+fifth_column+'</td></tr> </table> \n</div>\n</body></html>'

    outfile.write(html_file)

    infile.close()
    outfile.close()

    print "Wrote", srcHtmlDir+"/index.html"

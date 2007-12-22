#!/s/std/bin/python -O 

######################################
#
# Author: Lydia Sidrak, 11/2007
#
# Adds the "UI" portion of the visualization
# to each html file in the given directory.
# This is also known as the statistics toggling and
# scale on the side.
#
######################################

from copy import deepcopy
import sys
import os
import string               

debug = 0

def putDiv(htmlOutFile):

    # Opens the given file for reading
    infile = open(htmlOutFile, "r")
    #fname, extenstion = htmlOutFile.split(".html")
    #htmlNewFile = fname+"_NEW.html"
    #print htmlNewFile
    new_html = ""

    # Makes a list of each line in the report file     
    html_file = infile.read()
    prebody, restofbody = html_file.split("<body>") 
    # prebody is everything before <body>
    new_html = prebody +"<body>"
    
    #wrap div tag of source between body
    new_html = new_html + "<div id=\"source\">"     
    
    #add rest of body (source code) text
    middlebody, endbit = restofbody.split("</body>") #get the rest of the body content before </body
    new_html = new_html + middlebody
    new_html = new_html + "</div>" #closing div tag 
     
    #add menu
    new_html = new_html + "<div id=\"sidebar\">\n"
    new_html = new_html + "<table><tr><td><a href=\"#\" onclick=\"setActiveStyleSheet('coverage'); return false;\">coverage </a></td></tr>\n"
    new_html = new_html + "<tr><td><a href=\"#\" onclick=\"setActiveStyleSheet('crash-impact'); return false;\">impact view</td></tr>"
    new_html = new_html + "<tr><td><a href=\"#\" onclick=\"setActiveStyleSheet('crashiness'); return false;\">crash view </a></td></tr></table>\n"

    new_html = new_html + "<table><tr><td>"
    new_html = new_html + "<table> <tr> <td width=\"20\" height=\"5\" title=\"coverage 100th percentile\" bgcolor=\"#00ff00\">&nbsp;</td> </tr>\n"
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 90th\" bgcolor=\"#84ff84\">&nbsp;</td></tr>\n"
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 80th\" bgcolor=\"#a8ffa8\">&nbsp;</td></tr>\n"
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 70th\" bgcolor=\"#bdffbd\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 60th\" bgcolor=\"#cdffcd\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 50th\" bgcolor=\"#d9ffd9\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 40th\" bgcolor=\"#e3ffe3\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 30th\" bgcolor=\"#ebffeb\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 20th\" bgcolor=\"#f2fff2\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 10th\" bgcolor=\"#f9fff9\">&nbsp;</td></tr>  \n"
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"coverage 0th\" bgcolor=\"#fefffe\">&nbsp;</td></tr>  \n"
    new_html = new_html + "</table>	</td> \n	<td> \n	<table> "
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"crash-impact 100th percentile\" bgcolor=\"#00ffff\">&nbsp;</td></tr> \n " 
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"crash-impact 90th\" bgcolor=\"#84ffff\">&nbsp;</td></tr> \n " 
    new_html = new_html + "<tr> <td width=\"20\" height=\"5\" title=\"crash-impact 80th\" bgcolor=\"#a8ffff\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crash-impact 70th\" bgcolor=\"#bdffff\">&nbsp;</td></tr> \n " 
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crash-impact 60th\" bgcolor=\"#cdffff\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crash-impact 50th\" bgcolor=\"#d9ffff\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crash-impact 40th\" bgcolor=\"#e3ffff\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crash-impact 30th\" bgcolor=\"#ebffff\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crash-impact 20th\" bgcolor=\"#f2ffff\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crash-impact 10th\" bgcolor=\"#f9ffff\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crash-impact 0th\" bgcolor=\"#feffff\">&nbsp;</td></tr> \n "
    new_html = new_html +"</table>\n"
    new_html = new_html + "	</td><td> \n	<table>\n"
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 100th percentile\" bgcolor=\"#ffff00\">&nbsp;</td></tr> \n " 
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 90th\" bgcolor=\"#ffff84\">&nbsp;</td></tr> \n "  
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 80th\" bgcolor=\"#ffffa8\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 70th\" bgcolor=\"#ffffbd\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 60th\" bgcolor=\"#ffffcd\">&nbsp;</td></tr> \n " 
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 50th\" bgcolor=\"#ffffd9\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 40th\" bgcolor=\"#ffffe3\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 30th\" bgcolor=\"#ffffeb\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 20th\" bgcolor=\"#fffff2\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 10th\" bgcolor=\"#fffff9\">&nbsp;</td></tr> \n "
    new_html = new_html + "<tr>  <td width=\"20\" height=\"5\" title=\"crashiness 0th\" bgcolor=\"#fffffe\">&nbsp;</td></tr> \n "
    new_html = new_html + " </table> \n</td> \n"
    new_html = new_html + "<td> <table> <tr> <td width=\"5\" height=\"5\"> 100 </td> </tr>\n"
    new_html = new_html + "<tr> <td width=\"5\" height=\"160\"> </td> </tr>\n"
    new_html = new_html + "<tr> <td width=\"5\" height=\"5\"> 0 </td> </tr>\n"
    new_html = new_html + "</table>	</td> \n	<td>\n</tr> \n</table>\n</div>\n"
    new_html = new_html + "</body>" # closing body tag
    new_html = new_html + endbit 

    #Close the html file, then open it again to be re-written
    infile.close()
    outfile = open(htmlOutFile, "w")
    outfile.write(new_html)
    outfile.close()


######################################
#
# Adds the "UI" portion of the visualization
# to each html file in the given directory.
# This is also known as the statistics toggling and
# scale on the side.
#
######################################
def main():
    
    if len(sys.argv) < 2:
        print "Error. Html root needed."
        sys.exit()
 
    htmlRootDir = sys.argv[1]
 
    if not os.path.isdir(htmlRootDir):
        print "Error  " +htmlRootDir+ " path does not exist"
        exit(1)

    for root, dirs, files in os.walk(htmlRootDir): 
            for file in files: 
                if file.endswith(".html") and not (file == "index.html"):
                    htmlOutFile = root+"/"+file
                    if debug > 0:
                        print htmlOutFile
                    putDiv(htmlOutFile) 
  
if __name__ == '__main__':
    main()

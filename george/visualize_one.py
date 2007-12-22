#!/s/std/bin/python -O 

######################################
#
# Author: Jason Fletchall, 12/2007
#
# This script runs all the scripts to generate
# the George visualization for the given release.
#
# Usage: visualize_one.py reportsDir sitesDir vizOutDir srcHtmlDir distribution release
#        or
#        visualize_one.py -esp resultsDir
#
# Example: visualize_one.py visualize_one.py /tmp/fletchal/results/fedora-5-i386/nautilus-2.14.1-1.fc5.1.sam.1/data /tmp/fletchal/results/fedora-5-i386/nautilus-2.14.1-1.fc5.1.sam.1/sites scratch /tmp/fletchal/results/fedora-5-i386/nautilus-2.14.1-1.fc5.1.sam.1/src fedora-5-i386 nautilus-2.14.1-1.fc5.1.sam.1
#
######################################

import sys
import os
import string
import commands
from pyPgSQL import PgSQL
sys.path.append("./statistics")
import calculate_statistics
import generate_plots
sys.path.append("./utils")
sys.path.append("./html")
import wrapDiv
from make_menu import make_menu

debug = 0


def main():

    if len(sys.argv) != 7 and len(sys.argv) != 9 and len(sys.argv) != 3:
        print "Usage: visualize_one.py reportsDir sitesDir vizOutDir srcHtmlDir distribution release [-c copySrcDir]\n" + "Where -c indicates that the source html needs to be refreshed or copied from copySrcDir."
        print "Alternatively, you may run this as:\n" + "visualize_one.py -esp resultsDir\n" + "To activate ESP mode (the tool will guess everything based on CBI defaults)"
        return

    if sys.argv[1] == "-esp":
        #Figures out all the parameters based on CBI defaults
        resultsDir = sys.argv[2]
        if not resultsDir.endswith("/"):
            resultsDir += "/"
        reportsDir = resultsDir + "data"
        sitesDir = resultsDir + "sites"
        vizOutDir = resultsDir
        srcHtmlDir = resultsDir + "src"
        junk, distribution, release, junk2 = resultsDir.rsplit("/", 3)
    else:
        reportsDir = sys.argv[1]
        sitesDir = sys.argv[2]
        vizOutDir = sys.argv[3]
        srcHtmlDir = sys.argv[4]
        distribution = sys.argv[5]
        release = sys.argv[6]
    copySrcDir = ""
    if len(sys.argv) == 9:
        if sys.argv[7] != "-c":
            print "Usage: visualize_one reportsDir sitesDir vizOutDir srcHtmlDir distribution release [-c copySrcDir]\n" + "Where -c indicates that the source html needs to be refreshed or copied from copySrcDir."
            return
        copySrcDir = sys.argv[8]


    #Calculates statistics - creates a .viz file
    sys.argv ="junk",reportsDir,sitesDir,vizOutDir,srcHtmlDir,distribution,release  
    vizFileName = calculate_statistics.main()
    #vizFileName = "/tmp/fletchal/results/fedora-8-i386/nautilus-2.20.0-6.fc8.sam.1/fedora-8-i386___nautilus-2.20.0-6.fc8.sam.1.viz"

    #Refreshes the source html files if needed
    if copySrcDir != "":
        print "Copying html..."
        print commands.getoutput("utils/copy_dir_files " + copySrcDir + " " + srcHtmlDir)

    #Inserts css tags into each source code html file to prep for coloring
    print "Modifying html..."
    print commands.getoutput("html/put_css " + srcHtmlDir)

    #Generates the css files for coloring
    print "Generating highlighting..."
    if 'Main.class' not in os.listdir('.'):
        commands.getoutput("javac Main.java")
    print commands.getoutput("java Main " + vizFileName)

    #Adds statistics toggle & scale to the right frame in each source code html file
    print "Generating UI..."
    sys.argv = "junk", srcHtmlDir
    wrapDiv.main()
    if 'george' not in os.listdir('/tmp'):
        commands.getoutput("mkdir /tmp/george")
    if 'styleswitcher.js' not in os.listdir('/tmp/george'):
        commands.getoutput("cp html/styleswitcher.js /tmp/george")
    if 'menu.css' not in os.listdir('/tmp/george'):
        commands.getoutput("cp html/menu.css /tmp/george")

    #Picks up the .viz file and creates an html overview page
    make_menu(vizOutDir, srcHtmlDir, distribution, release)
    if 'menu.css' not in os.listdir(srcHtmlDir):
        commands.getoutput("cp html/menu.css " + srcHtmlDir)
    if 'maintable.css' not in os.listdir(srcHtmlDir):
        commands.getoutput("cp html/maintable.css " + srcHtmlDir)
    
    print "Profit!"

    if debug > 1:
        commands.getoutput("firefox " + srcHtmlDir + "/index.html")


if __name__ == '__main__':
    main()

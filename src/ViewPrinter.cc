#include <iostream>
#include <sstream>
#include "ViewPrinter.h"

using namespace std;


ViewPrinter::ViewPrinter(const char *stylesheet, const char dtd[], const char filename[])
{
  init(filename, stylesheet, dtd);

  (*this) << "<scores>";
}


ViewPrinter::ViewPrinter(const char *stylesheet, const char dtd[], const string &scheme, const char sort[], const char projection[])
{
  ostringstream filename;
  filename << scheme << '_' << sort << '_' << projection << ".xml";

  init(filename.str().c_str(), stylesheet, dtd);

  (*this) << "<scores scheme=\"" << scheme
	  << "\" sort=\"" << sort
	  << "\" projection=\"" << projection
	  << "\">";
}


void
ViewPrinter::init(const char filename[], const char stylesheet[], const char dtd[])
{
  exceptions(ios::eofbit | ios::failbit | ios::badbit);
  open(filename);

  (*this) << "<?xml version=\"1.0\"?>"
	  << "<?xml-stylesheet type=\"text/xsl\" href=\"" << stylesheet << "\"?>"
	  << "<!DOCTYPE scores SYSTEM \"" << dtd << ".dtd\">";
}


ViewPrinter::~ViewPrinter()
{
  (*this) << "</scores>\n";
}

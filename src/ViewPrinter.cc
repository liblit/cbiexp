#include <iostream>
#include <sstream>
#include "ViewPrinter.h"

using namespace std;


ViewPrinter::ViewPrinter(const char dtd[], const string &scheme, const char sort[], const char projection[])
{
  ostringstream filename;
  filename << scheme << '_' << sort << '_' << projection << ".xml";

  exceptions(ios::eofbit | ios::failbit | ios::badbit);
  open(filename.str().c_str());

  (*this) << "<?xml version=\"1.0\"?>"
	  << "<?xml-stylesheet type=\"text/xsl\" href=\"" << dtd << ".xsl\"?>"
	  << "<!DOCTYPE view SYSTEM \"" << dtd << ".dtd\">"
	  << "<view scheme=\"" << scheme
	  << "\" sort=\"" << sort
	  << "\" projection=\"" << projection
	  << "\">";
}


ViewPrinter::~ViewPrinter()
{
  (*this) << "</view>\n";
}

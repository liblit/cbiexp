#include <sstream>
#include "xml.h"

using std::ostringstream;
using std::string;


string
xml::escape(const string &raw)
{
  // escaping strategy:
  //
  // - the three XML reserved characters (<, >, &) are escaped numerically
  //
  // - single and double quote are escaped numerically; this is useful
  //   for attribute values and is unnecessary but harmless elsewhere
  //
  // - non-ASCII characters (>= 128) are escaped numerically
  //
  // - non-printing characters are escaped numerically
  //
  // - all other printing characters are unescaped

  const size_t length = raw.length();
  ostringstream result;

  for (size_t pos = 0; pos < length; ++pos)
    {
      const unsigned char byte = raw[pos];
      switch (byte)
	{
	default:
	  if (byte < 127 && isprint(byte))
	    result << byte;
	  else
	  case '<':
	  case '>':
	  case '&':
	  case '\'':
	  case '"':
	    result << "&#" << unsigned(byte) << ';';
	}
    }

  return result.str();
}

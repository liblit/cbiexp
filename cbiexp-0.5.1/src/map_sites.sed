#!/bin/sed -f

# don't make any changes at all to XML-like tag lines
/^</ b

# remove function name prefix on local variables
s/[a-zA-Z_][a-zA-Z_0-9]*\$//g

# remove long long integer constant suffixes
s/\([0-9][0-9]*\)LL/\1/g
s/\([0-9][0-9]*\)ULL/\1/g

# backslash-escape backslashes
s/\\/\\\\/g

# backslash-escape quotation marks,
s/\"/\\\"/g

# entity-escape special XML characters
s/&/\&amp;/g
s/</\&lt;/g
s/>/\&gt;/g

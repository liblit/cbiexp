#!/bin/sed -f

# remove function name prefix on local variables
s/[a-zA-Z_][a-zA-Z_0-9]*\$//g

# remove long long integer constant suffixes
s/\([0-9][0-9]*\)LL/\1/g
s/\([0-9][0-9]*\)ULL/\1/g

# backslash-escape quotation marks,
# but not on XML-like tag lines
/^</! s/\"/\\\"/g

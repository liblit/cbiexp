#! /bin/sh
# Check that the ccrypt program works properly.

if [ -z "$srcdir" ]; then
    srcdir=.
fi

NAME=$0

KEY=testkey
CCRYPT="../src/ccrypt -f -K $KEY"
DATA=$srcdir/testdata
CPTDATA=$srcdir/testdata.cpt
UCPTDATA=$srcdir/testdata.ucpt
TMP1=./tmp1
TMP2=./tmp2

action () {
    $@
    if [ $? -ne 0 ]; then
	echo $NAME: test failed. >&2
	exit 1
    fi
}

decrypttest() {
    D=$1
    C=$2

    action $CCRYPT -d < $C > $TMP1
    action diff $D $TMP1 > /dev/null
    action rm -f $TMP1

    action cp $C $TMP1.cpt
    action $CCRYPT -d $TMP1.cpt
    action diff $D $TMP1
    action rm -f $TMP1
}

decrypttest $DATA $CPTDATA

action $CCRYPT < $DATA > $TMP2

decrypttest $DATA $TMP2
action rm -f $TMP2

action cp $DATA $TMP2
action $CCRYPT $TMP2

decrypttest $DATA $TMP2.cpt
action rm -f $TMP2.cpt

action $CCRYPT -u < $UCPTDATA > $TMP2
action diff $TMP2 $DATA
action rm -f $TMP2

action $CCRYPT -u $UCPTDATA > $TMP2
action diff $TMP2 $DATA
action rm -f $TMP2

echo $NAME: test succeeded >&2
exit 0

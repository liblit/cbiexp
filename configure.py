#!/usr/bin/python

from sys import argv, exit
from subprocess import call

command = ['./configure', 'GXX=/s/gcc-4.3.2/bin/g++']
command += argv[1:]

exit(call(command))

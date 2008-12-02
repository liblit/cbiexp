# Set the location of the CodeSurfer C API include files.
H_INCLUDE = $(CSURF_ROOT)/csurf/include

pkglib_LTLIBRARIES = libeffort.la

noinst_SCRIPTS = effort.stk
CLEANFILES = effort.stk

# Set the csurf executable.
# If the path to GrammaTech/CodeSurfer/bin (or wherever you installed CodeSurfer)
# isn't in your path, you can override the CSURF variable with an absolute path
# to the csurf executable.
CSURF = $(CSURF_ROOT)/csurf
CSURF_COMPILER = $(CC)

# Set the plugin directory
DIR := `pwd`

effort.stk:
	@rm -f $(DIR)/effort.stk
	@echo "(load \"$(DIR)/.libs/libeffort.so\")" >> \
		effort.stk
	@echo "(quit)" >> effort.stk

libeffort_la_SOURCES = effort.cc
libeffort_la_LIBADD = ../libanalyze.a
libeffort_la_CPPFLAGS = -W -Wall -Wformat -I$(H_INCLUDE) -I../..

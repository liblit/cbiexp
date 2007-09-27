CSURF_ROOT = /p/cbi/public/codesurfer
# Set the location of the C API include files and codesurfer.lib (codesurfer.lib
# is unnecessary when compiling with gcc).
H_INCLUDE = $(CSURF_ROOT)/api/include
CODESURFER_LIB = $(CSURF_ROOT)/lib/codesurfer.lib

pkglib_LTLIBRARIES =				\
	libpred_distance.la

noinst_SCRIPTS =				\
	pred_distance_load.stk

# Set the csurf executable.
# If the path to GrammaTech/CodeSurfer/bin (or wherever you installed CodeSurfer)
# isn't in your path, you can override the CSURF variable with an absolute path
# to the csurf executable.
CSURF = csurf
CSURF_COMPILER = $(CC)

# Set the plugin directory
DIR := `pwd`

pred_distance_load.stk:
	@rm -f $(DIR)/pred_distance_load.stk
	@echo "(load \"$(DIR)/.libs/libpred_distance.so\")" >> \
		pred_distance_load.stk
	@echo "(quit)" >> pred_distance_load.stk

libpred_distance_la_SOURCES =				\
	pred_distance.cc

libpred_distance_la_LIBADD =				\
	../libanalyze.a
        
# TODO Add -Werror and fix the resulting compiler error
libpred_distance_la_CPPFLAGS = -W -Wall -Wformat -I$(H_INCLUDE) -I../..

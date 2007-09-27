CSURF_ROOT = /p/cbi/public/codesurfer
# Set the location of the C API include files and codesurfer.lib (codesurfer.lib
# is unnecessary when compiling with gcc).
H_INCLUDE = $(CSURF_ROOT)/api/include
CODESURFER_LIB = $(CSURF_ROOT)/lib/codesurfer.lib

pkglib_LTLIBRARIES =				\
	libeffort.la

noinst_SCRIPTS =				\
	effort.stk

# Set the csurf executable.
# If the path to GrammaTech/CodeSurfer/bin (or wherever you installed CodeSurfer)
# isn't in your path, you can override the CSURF variable with an absolute path
# to the csurf executable.
CSURF = csurf
CSURF_COMPILER = $(CC)

# Set the plugin directory
DIR := `pwd`

effort.stk:
	@rm -f $(DIR)/effort.stk
	@echo "(load \"$(DIR)/.libs/libeffort.so\")" >> \
		effort.stk
	@echo "(quit)" >> effort.stk

libeffort_la_SOURCES =				\
	effort.cc

libeffort_la_LIBADD =				\
	../libanalyze.a
        
libeffort_la_CPPFLAGS = -W -Wall -Wformat -Werror -I$(H_INCLUDE) -I../..

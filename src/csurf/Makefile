
# Set the compiler
#
# Note: it is assumed the environment is set up to run cl on Windows or gcc 
#       on *nix.  If this is not the case, override the variable CC with
#       a valid compiler -- eg. "make tutorial CC=some_other_compiler".

ifeq ($(OS),Windows_NT)
CC = cl
else
CC = g++
endif
CFLAGS = 

CSURF_ROOT = /p/course/cs706-liblit/public/codesurfer
# Set the location of the C API include files and codesurfer.lib (codesurfer.lib
# is unnecessary when compiling with gcc).
H_INCLUDE = $(CSURF_ROOT)/api/include
CODESURFER_LIB = $(CSURF_ROOT)/lib/codesurfer.lib

# Set the csurf executable.
# If the path to GrammaTech/CodeSurfer/bin (or wherever you installed CodeSurfer)
# isn't in your path, you can override the CSURF variable with an absolute path
# to the csurf executable.
CSURF = csurf
CSURF_COMPILER = $(CC)

# The plugin.
PLUGIN = pred_distance

# Set the tutorial directory
DIR := `pwd`
ifeq ($(OS),Windows_NT)
DIR_WIN := $(shell cygpath -w $(DIR) | \
	/bin/sed -e 's@\\@\\\\@g')
endif

ifeq ($(OS),Windows_NT)
all: FORCE build-dll stk-load-file	
else
all: FORCE build-so stk-load-file	
endif

stk-load-file:
	@rm -f $(PLUGIN)_load.stk
ifeq ($(OS),Windows_NT) # change: arumuga.  Moved the ifeq from after the command
	@echo "(set! *load-path* (cons \"$(DIR_WIN)\" *load-path*))" > \
		$(PLUGIN)_load.stk
	@echo "(load \"$(DIR)/$(PLUGIN).dll\")" >> \
		$(PLUGIN)_load.stk
else
	@echo "(load \"$(DIR)/$(PLUGIN).so\")" >> \
		$(PLUGIN)_load.stk
endif
	@echo "(quit)" >> $(PLUGIN)_load.stk

build-dll:
	@$(CC) /LD $(CFLAGS) /I$(H_INCLUDE) \
		$(PLUGIN).c $(CODESURFER_LIB)

build-so:
	@$(CC) -shared -I$(H_INCLUDE) \
		$(PLUGIN).cc -o $(PLUGIN).so

clean:
	rm - f *.o

.PHONY: FORCE

FORCE: 


########################################################################
# This inner makefile actually does the work of the compilation.
# See line 50 for the only reason this makefile should be edited (probably).
########################################################################

# STEP 1: Create variables used
# why so many warnings flags? because it's good practice to be warning and error free
# the -Werror flag turns any compiler warnings into errors
C_FLAGS   = -c -std=c99 -pedantic -Wall -Wextra -D__EXTENSIONS__
OPT       = -O3 -Werror
D_FLAG    = -g
LD_FLAGS  = -Wall -Wextra -Werror
EXE       = notarookie
TEST      = stresstest
GCC       = gcc
RM        = rm

# This section determines which tools should be used for debugging
# memory checking, depending on the system. Windows users, I'm sorry.
UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
	DEBUGGER=gdb
	LEAKER=valgrind --leak-check=full --read-var-info=yes --show-reachable=yes
else
ifeq ($(UNAME),Darwin)
	DEBUGGER=lldb
	LEAKER=leaks -quiet -atExit --
endif
endif

# These are the C header files used.
HEADERS   = constants.h functions.h macros.h enums.h

# These are the C source files for the engine executable
SRCS      = init.c engine.c bboard.c hashkey.c board.c consts.c attack.c \
            printer.c movegen.c validation.c makemove.c perft.c search.c util.c \
	    hashset.c evaluate.c uci.c xboard.c console.c

# These are the C source files for the perft testing executable
TEST_SRCS = init.c stresstest.c bboard.c hashkey.c board.c consts.c attack.c \
            printer.c movegen.c validation.c makemove.c perft.c

# Generic variable for converting the engine source files into object files
OBJS      = $(SRCS:.c=.o)

# Generic variable for converting the perft test source files into object files
TEST_OBJS = $(TEST_SRCS:.c=.o)


# NOTE: In the below makefile targets, replace $(D_FLAG) with $(OPT)
# to enable optimizations and turn off debugging info

# Generic (hidden) target for compiling C files into object files
# Used by the OBJS and TEST_OBJS targets below
.c.o:
	@echo "Compiling each C source file separately..."
	$(GCC) $(C_FLAGS) $(D_FLAG) $<
	@echo ""

# Target for creating the engine exectuable
# First compiles all object files, then links them together,
# moves the exectuable to the bin directory, and echos the results out.
$(EXE): $(OBJS)
	@echo "Creating executable..."
	$(GCC) -o $(EXE) $(LD_FLAGS) $(D_FLAG) $(OBJS)
	@mv $(EXE) ../bin
	@echo ""
	@echo "Compilation successful!"

# Target for creating the perft testing exectuable
# First compiles all object files, then links them together,
# moves the exectuable to the bin directory, and echos the results out.
$(TEST): $(TEST_OBJS)
	@echo "Creating executable..."
	$(GCC) -o $(TEST) $(LD_FLAGS) $(D_FLAG) $(TEST_OBJS)
	@mv $(TEST) ../bin
	@echo ""
	@echo "Compilation successful!"

# Target for compiling the engine source files
# First creates the bin directory (see `start` target below)
# then uses the generic .c.o target above to compile each C
# file one by one
$(OBJS): start $(HEADERS)

# Target for compiling the perft testing source files
# First creates the bin directory (see `start` target below)
# then uses the generic .c.o target above to compile each C
# file one by one
$(TEST_OBJS): start $(HEADERS)

# Target that creates the bin directory (if it doesn't already exist)
start:
	@[ -d ../bin ] || mkdir ../bin

# Target to run the debug tool on the engine executable
# Compiles the engine first
debug: $(EXE)
	$(DEBUGGER) ../bin/$(EXE)

# Target to run the memory safety tool on the engine executable
# Compiles the engine first
memcheck: $(EXE)
	$(LEAKER) ../bin/$(EXE)

# Target to reset the directories. Removes the bin directory
# (and everything in it) and removes any other lingering,
# unnecessary files
clean:
	@echo "Cleaning up"
	$(RM) -rf ../bin *.o .DS_Store ../*.debug
	@echo ""
	@echo "Clean."

# Recompile the engine from scratch
# First removes everything via the `clean` target, then recompiles
new:
	make clean
	make
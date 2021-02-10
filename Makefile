########################################################################
# Makefile for interacting with the makefile inside of the
# directory with the actual code in it. This is purely for
# convenience of not having to `cd` into the inner directory.
########################################################################

EXE       = notarookie
TEST      = stresstest

# Create the actual engine executable
# calls the inner makefile target (see other file for explanation)
$(EXE):
	@make -C ./notarook-ie/ $(EXE)

# create the perft test executable
# calls the inner makefile target (see other file for explanation)
$(TEST):
	@make -C ./notarook-ie/ $(TEST)

# Create and run the engine executable in a command line debugger
# calls the inner makefile target (see other file for explanation)
debug:
	@make -C ./notarook-ie/ debug

# Create and run the engine exectuable in a memory safety checker
# calls the inner makefile target (see other file for explanation)
memcheck:
	@make -C ./notarook-ie/ memcheck

# Clean up everything
# calls the inner makefile target (see other file for explanation)
clean:
	@make -C ./notarook-ie/ clean

# Recompile the engine from scratch
# calls the inner makefile target (see other file for explanation)
new:
	@make -C ./notarook-ie/ new
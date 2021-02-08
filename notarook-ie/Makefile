C_FLAGS   = -c -std=c99 -pedantic -Wall -D__EXTENSIONS__
OPT       = -O3
D_FLAG    = -g
LD_FLAGS  = -Wall
EXE       = notarookie
TEST      = stresstest
GCC       = gcc
RM        = rm

HEADERS   = constants.h functions.h macros.h enums.h
SRCS      = init.c engine.c bboard.c hashkey.c board.c consts.c attack.c \
           printer.c movegen.c validation.c makemove.c perft.c
TEST_SRCS = init.c stresstest.c bboard.c hashkey.c board.c consts.c attack.c \
           printer.c movegen.c validation.c makemove.c perft.c
OBJS      = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

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

# NOTE: In the below makefile targets, replace $(D_FLAG) with $(OPT)
# to enable optimizations and turn off debugging info

.c.o:
	@echo "Compiling each C source file separately..."
	$(GCC) $(C_FLAGS) $(D_FLAG) $<
	@echo ""

$(EXE): $(OBJS)
	@echo "Linking all object modules..."
	$(GCC) -o $(EXE) $(LD_FLAGS) $(D_FLAG) $(OBJS)
	@echo ""
	@echo "Compilation successful!"

$(TEST): $(TEST_OBJS)
	@echo "Linking all object modules..."
	$(GCC) -o $(TEST) $(LD_FLAGS) $(D_FLAG) $(TEST_OBJS)
	@echo ""
	@echo "Compilation successful!"

$(OBJS): $(HEADERS)

$(TEST_OBJS): $(HEADERS)

debug: $(EXE)
	$(DEBUGGER) $(EXE)

memcheck: $(EXE)
	$(LEAKER) ./$(EXE)

clean:
	@echo "Cleaning up"
	$(RM) -f *.o $(EXE) $(TEST) .DS_Store
	@echo ""
	@echo "Clean."

new:
	make clean
	make
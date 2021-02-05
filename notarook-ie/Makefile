C_FLAGS  = -c -g -std=c99 -pedantic -Wall -D__EXTENSIONS__
LD_FLAGS = -g -Wall
EXE      = notarookie
GCC      = gcc
RM       = rm

HEADERS  =
SRCS     =
OBJS     = $(SRCS:.c=.o)

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

.c.o:
	@echo "Compiling each C source file separately..."
	$(GCC) $(C_FLAGS) $<
	@echo ""

$(EXE): $(OBJS)
	@echo "Linking all object modules..."
	$(GCC) -o $(EXE) $(LD_FLAGS) $(OBJS)
	@echo ""
	@echo "Compilation successful!"

$(OBJS): $(HEADERS)

debug: $(EXE)
	$(DEBUGGER) $(EXE)

memcheck: $(EXE)
	$(LEAKER) $(EXE)

clean:
	@echo "Cleaning up"
	$(RM) -f bin/*.o $(EXE) .DS_Store
	@echo ""
	@echo "Clean."

new:
	make clean
	make
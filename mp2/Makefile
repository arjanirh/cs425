# Lines that start with a # are comments

OBJS = node.o listener.o 
EXE1 = node
EXE2 = listener

COMPILER = g++
COMPILER_FLAGS = -c -g -O0 -Wall -Wparentheses -Wreturn-type -Wmissing-braces
LINKER = g++
LINKER_FLAGS = -lpthread

all : clear $(EXE1) $(EXE2)

$(EXE1) : node.o
	$(LINKER) node.o $(LINKER_FLAGS) -o $(EXE1)

node.o : node.cpp 
	$(COMPILER) $(COMPILER_FLAGS) node.cpp
 		
$(EXE2) : listener.o
	$(LINKER) listener.o $(LINKER_FLAGS) -o $(EXE2)

listener.o : listener.cpp 
	$(COMPILER) $(COMPILER_FLAGS) listener.cpp

clean : 
	rm -f *.o $(EXE1) $(EXE2) 2>/dev/null

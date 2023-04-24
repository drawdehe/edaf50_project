# Define the compiler and the linker. The linker must be defined since
# the implicit rule for linking uses CC as the linker. g++ can be
# changed to clang++.
CXX = g++
CC  = $(CXX)

# Generate dependencies in *.d files
DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

# Define preprocessor, compiler, and linker flags. Uncomment the # lines
# if you use clang++ and wish to use libc++ instead of GNU's libstdc++.
# -g is for debugging.
CPPFLAGS =  -std=c++11 -I.
CXXFLAGS =  -O2 -Wall -Wextra -pedantic-errors -Wold-style-cast 
CXXFLAGS += -std=c++11 
CXXFLAGS += -g
CXXFLAGS += $(DEPFLAGS)
LDFLAGS =   -g 
#CPPFLAGS += -stdlib=libc++
#CXXFLAGS += -stdlib=libc++
#LDFLAGS +=  -stdlib=libc++

# Targets
PROGS =  src/articletest src/grouptest src/primarytest src/client

all: $(PROGS)

test: 

# Targets rely on implicit rules for compiling and linking

articletest: src/articletest.o
grouptest: src/grouptest.o
primarytest: src/primarytest.o
client: src/client.o

# Phony targets
.PHONY: all test clean distclean

# Standard clean
clean:
	rm -f src/*.o $(PROGS)

distclean: clean
	rm src/*.d

# Include the *.d files
SRC = $(wildcard *.cc)
-include $(SRC:.cc=.d)

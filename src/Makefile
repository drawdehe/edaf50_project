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
CXXFLAGS =  -O0 -Wall -Wextra -pedantic-errors -Wold-style-cast 
CXXFLAGS += -std=c++11 
CXXFLAGS += -g
CXXFLAGS += $(DEPFLAGS)
LDFLAGS =   -g 
#CPPFLAGS += -stdlib=libc++
#CXXFLAGS += -stdlib=libc++
#LDFLAGS +=  -stdlib=libc++

# Targets

PROGS =  client primaryserver diskserver #disktest primarytest articletest grouptest primarytest

all: $(PROGS)

test: 

# Targets rely on implicit rules for compiling and linking


client: client.o messagehandler.o connection.o
primaryserver: primaryserver.o article.o connection.o messagehandler.o
diskserver: diskserver.o article.o connection.o messagehandler.o
# disktest: disktest.o article.o
# primarytest: primarytest.o article.o
# articletest: articletest.o
# grouptest: grouptest.o

# Phony targets
.PHONY: all test clean distclean

# Standard clean
clean:
	rm -f ../bin/$(PROGS)
	rm -f *.o $(PROGS)

distclean: clean
	rm *.d

cleardatabase:
	rm -fr ./database

install: 
	make
	mkdir -p ../bin
	install $(PROGS) ../bin

# Include the *.d files
SRC = $(wildcard *.cc)
-include $(SRC:.cc=.d)

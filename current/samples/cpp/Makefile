# Makefile for c/urg/samples
# $Id$

# Compile options
SRCDIR = ../../src
INCLUDEDIR = ../../include/cpp
CC = gcc
CXX = g++
CFLAGS = -g -O0 -Wall -W $(INCLUDES) -lcurl -lcrypto -lpthread
CXXFLAGS = $(CFLAGS)
INCLUDES = -I$(INCLUDEDIR)
LDFLAGS =
LDLIBS = -lm $(shell if test `echo $(OS) | grep Windows`; then echo "-lwsock32 -lsetupapi"; else if test `uname -s | grep Darwin`; then echo ""; else echo "-lrt"; fi; fi) -L$(SRCDIR)

# Target
TARGET = \	
	URG_touch_screen \
	Controller \
	Service \
	URG_server \
	

all : $(TARGET)

clean :
	$(RM) *.o $(TARGET) *.exe

depend :
	makedepend -Y -- $(INCLUDES) -- $(wildcard *.h *.c)

.PHONY : all depend clean

######################################################################
REQUIRE_LIB = $(SRCDIR)/liburg_cpp.a
$(REQUIRE_LIB) : $(wildcard $(SRCDIR)/*.[ch])
	cd $(@D)/ && $(MAKE) $(@F)

#URG_touch_screen : Connection_information.o $(REQUIRE_LIB)
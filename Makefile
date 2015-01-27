CC = gcc
CXX = g++

# define compile flags
CFLAGS = -g -Wall -O2

# define link flags
#LDFALGS =

# macro definitions
#DFLAGS =

# libraries to link
# LIBS = -lm -lz

# add include search path
#INCLUDES = -I. -I/foo/bar/include

# add library search path
#LIBPATH = -L. -L/foo/bar/lib

OBJS = anyarg.o example.o

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: all clean
all: $(OBJS) taa

taa: example.o anyarg.o
	$(CXX) $(CFLAGS) -o $@ $^

anyarg.o: anyarg.h
example.o: anyarg.h

clean:
	rm $(OBJS) taa


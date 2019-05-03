#******************************************************************************
# TARGETS : haploblocks
# AUTHOR  : Jens Stoye
# DATE    : 06/2018
#******************************************************************************

#compiler
CC = gcc

#compiler options
CFLAGS = -Ofast -I include -I io
#CFLAGS = -g -Wall -I include -I io
LDFLAGS =

#programs
ALL = haploblocks
OTHERS = 

#pattern rules
%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

#targets
all: $(ALL) 

#------------------------------------------------------------------------------
haploblocks: haploblocks.o binarymatrix.o lazytree.o io/io.o
	$(CC) $(LDFLAGS) $^ -o $@

#------------------------------------------------------------------------------
clean:
	-/bin/rm -f *.o io/*.o

remove: clean
	-/bin/rm -i $(ALL)

#******************************************************************************

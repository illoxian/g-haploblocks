# compiler
CC = g++
# compiler flags
CFLAGS = -std=c++14 -Wall -g -O3
# programs
ALL = vcf2bm

all: $(ALL)

# executables
vcf2bm: vcf2bm.o utils.o
	$(CC) $(CFLAGS) vcf2bm.o utils.o -o vcf2bm

# out files
vcf2bm.o: vcf2bm.cpp
	$(CC) $(CFLAGS) -c vcf2bm.cpp
utils.o: utils.cpp utils.h
	$(CC) $(CFLAGS) -c utils.cpp

# clean
clean:
	rm *.o

#remove
remove: clean
	rm -i $(ALL)


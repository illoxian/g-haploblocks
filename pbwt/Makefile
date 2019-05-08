#compiler
CXX = g++

#compiler options
CXXFLAGS = -Ofast -std=c++11

#programs
ALL = haplotype-pbwt-lite


#targets
all: $(ALL) 

#------------------------------------------------------------------------------
haplotype-pbwt-lite: haplotype-pbwt-lite.cpp
	$(CXX) $(CXXFLAGS)  haplotype-pbwt-lite.cpp -o haplotype-pbwt-lite


remove: 
	-/bin/rm -i $(ALL)

#******************************************************************************

CXX = g++
CXXFLAGS = -std=c++11
SRC_FILES = $(wildcard *.cpp)
EXECUTABLES = $(SRC_FILES:.cpp=)

all: $(EXECUTABLES)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLES)
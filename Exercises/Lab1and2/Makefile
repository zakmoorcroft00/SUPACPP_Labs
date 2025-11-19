# Simple Makefile to build the project in a Codespace or locally.

CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -pedantic # pedantic strictly standard-conforming code
# Executable name
BIN := AnalyseData

# Sources and header
SRCS := AnalyseData_5.cxx CustomFunctions.cxx
HDRS := CustomFunctions.h

# Default target
all: $(BIN)

# Link step (also triggers compilation of .o files)
$(BIN): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

# Remove build artefacts
clean:
	rm -f $(BIN) *.o

# Convenience run target (optional filename and N can be passed: make run FILE="path" N=10)
run: $(BIN)
	./$(BIN) $(FILE) $(N)

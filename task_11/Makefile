CXX := g++-11
CXXFLAGS := -std=c++11 -Wall -Wextra

# Target for compiling in production mode
production: CXXFLAGS += -O3
production: solution.cpp expected.hpp optional.hpp
	$(CXX) $(CXXFLAGS) -o production_main solution.cpp

# Target for compiling in debug mode
debug: CXXFLAGS += -g -O0
debug: solution.cpp expected.hpp optional.hpp
	$(CXX) $(CXXFLAGS) -o debug_main solution.cpp

# Target for running valgrind on the debug executable
valgrind: debug
	valgrind ./debug_main

clean:
	rm -f production_main debug_main

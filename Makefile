CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

main: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o main

clean:
	rm -f main

.PHONY: clean
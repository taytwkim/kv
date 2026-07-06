CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

SHARED_SRC = src/store.cpp src/command.cpp src/response.cpp src/log.cpp

.PHONY: all run-repl run-server clean

all: repl server

repl: src/main.cpp $(SHARED_SRC)
	$(CXX) $(CXXFLAGS) src/main.cpp $(SHARED_SRC) -o repl

server: src/run.cpp src/server.cpp $(SHARED_SRC)
	$(CXX) $(CXXFLAGS) src/run.cpp src/server.cpp $(SHARED_SRC) -o server

run-repl: repl
	./repl

run-server: server
	./server

clean:
	rm -f repl server
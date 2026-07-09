CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# -pedantic warns us about code that is accepted by the 
# compiler but is not part of the standard C++ language.

SHARED_SRC = src/store.cpp src/command.cpp src/response.cpp src/log.cpp

# A Makefile rule has this general form:
#
# target: prerequisites
#	recipe
#
# The prerequisites are built or checked before the target.
# The recipe is the command used to build or run the target.

# These targets are not real files/executables.
# They are command names, so Make always run them when requested.
.PHONY: all run-repl run-server clean

# The first target is the default target.
# So running just "make" is the same as running "make all".
#
# This target asks Make to build both executables.
all: repl server

# Build the REPL executable.
# Rebuild it if main.cpp or any shared source file changed.
repl: src/main.cpp $(SHARED_SRC)
	$(CXX) $(CXXFLAGS) src/main.cpp $(SHARED_SRC) -o repl

# Build the server executable.
# Rebuild it if run.cpp, server.cpp, or any shared source file changed.
server: src/run.cpp src/server.cpp $(SHARED_SRC)
	$(CXX) $(CXXFLAGS) src/run.cpp src/server.cpp $(SHARED_SRC) -o server

# Build the REPL if needed, then run it.
run-repl: repl
	./repl

# Build the server if needed, then run it.
run-server: server
	./server

# Remove generated executables.
clean:
	rm -f repl server
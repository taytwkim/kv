CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

SRC = src/main.cpp src/store.cpp src/command.cpp src/response.cpp src/log.cpp
TARGET = store

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
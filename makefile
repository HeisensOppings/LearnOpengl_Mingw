CXX := g++
CXX_FLAGS := -g -std=c++17 -O2 -Wextra -Wall

SRC := src
INCLUDE := ./include
LIB := ./lib

LIBRARIES := -lglad -lglfw3dll
EXECUTABLE := main.exe

OBJECTS := $(patsubst $(SRC)/%.cpp, obj/%.o, $(wildcard $(SRC)/*.cpp))

all: ./$(EXECUTABLE)

run: all
	./$(EXECUTABLE)

./$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

obj/%.o: $(SRC)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -c $< -o $@
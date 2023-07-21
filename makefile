CXX := g++
CXX_FLAGS := -g -std=c++17 -O2 -Wextra -Wall -Wno-pragmas

SRC := ./src
INCLUDE := ./include
LIB := ./lib

LIBRARIES := -lglad -lglfw3dll -lassimp
EXECUTABLE := main.exe

SOURCES := $(wildcard $(SRC)/**/*.cpp $(SRC)/*.cpp)
HEADERS := $(wildcard $(INCLUDE)/*.h) $(wildcard $(SRC)/**/*.h)
OBJECTS := $(patsubst $(SRC)/%.cpp, obj/%.o, $(SOURCES))

ifeq ($(wildcard obj),)
$(shell mkdir obj)
endif

ifeq ($(wildcard obj\imgui),)
$(shell mkdir obj\imgui)
endif

all: ./$(EXECUTABLE)

run: all
	./$(EXECUTABLE)

./$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

obj/%.o: $(SRC)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -I$(SRC) -MMD -MP -c $< -o $@

-include $(OBJECTS:.o=.d)
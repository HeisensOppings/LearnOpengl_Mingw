#### LearnOpengl with mingw makefile in vscode

[English](README.md) [中文](README_zh.md)

##### Environment
- Cmake, Mingw32-make, Vscode

##### Build GLFW

- [Download](https://github.com/glfw/glfw/releases) the GLFW source code package and recompile it to make sure it runs on you own machine
- Unzip the source code and open CMake
- Input the source code and build(mkdir) directory
- Click Configure, Select MinGW Makefiles
- After Configure, Click BUILD_SHARED_LIBS, Create a dynamic library
- Click Generate
- In build directory run cmd $:mingw32-make ../
- Copy files in build/src/ **glfw3.dll**、**libglfw3dll.a**, and source code/include

##### Configure GLAD

- language - C/C++, API - higher than 3.3, Profile - Core, Generate a loader, Generate, [Download](https://glad.dav1d.de/) the zip
- gcc .\src\glad.c -c -I.\include\
- ar -rc libglad.a glad.o
- copy file **libglad.a** and include

###### Assimp

- [Download](https://github.com/assimp/assimp) Assimp source code and extract it to a local directory
- Use CMake to configure and generate build files for the Assimp source code, By default, the build directory will be generated
- Navigate to the build directory and use the mingw32-make command to compile the Assimp source code and generate executable files or libraries
- Put the build\lib\libassimp.dll.a file into lib/ directory
- Put the build\bin\libassimp-5.dll file into the same directory as the executable file(exe)

###### Loading Modes

- Assimp uses models in OBJ format. If the downloaded model is in PMX or other formats, you can import the model using the blender mmd_tools plugin [Download](https://github.com/powroupi/blender_mmd_tools) , and then export it as an OBJ format file. Remember to modify the texture file name in the MTL file. 

```
# example of mtl file
newmtl model_name
Ns xx.xxxxxx
Ka 0.000000 0.000000 0.000000
Kd 1.000000 1.000000 1.000000
Ks 0.500000 0.500000 0.500000
Ke 0.000000 0.000000 0.000000
Ni 1.000000
d 1.000000
illum 3
map_Kd diffuse_texture.png
map_Ks specular_texture.png
map_Bump bump_texture.png
```

###### Create makefile

- make sure formatting is correct, especially tabs and spaces

```makefile
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
```

###### Finally open terminal in vscode run command "mingw32-make | .\main.exe" to build project and run program or(windows system) you can ./build.bat to build and run program

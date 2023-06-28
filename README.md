#### LearnOpengl with mingw makefile in vscode

[English](README.md) [Chinese](README_zh.md)

##### Build GLFW

- [Download](https://github.com/glfw/glfw/releases) the GLFW source code package and recompile it to make sure it runs on you own machine
- Unzip the source code and open CMake
- Input the source code and build(mkdir) directory
- Click Configure, Select MinGW Makefiles
- After Configure, Click BUILD_SHARED_LIBS, Create a dynamic library
- Click Generate
- Copy files in build/src/ **glfw3.dll**、**libglfw3dll.a**, and source code/include

##### Configure GLAD

- language - C/C++, API - higher than 3.3, Profile - Core, Generate a loader, Generate, [Download](https://glad.dav1d.de/) the zip
- gcc .\src\glad.c -c -I.\include\
- ar -rc libglad.a glad.o
- copy file **libglad.a** and include

###### Create makefile

- make sure formatting is correct, especially tabs and spaces

```makefile
CXX		:= g++
CXX_FLAGS       := -g -std=c++17 #-Wextra -Wall

SRC		:= src
INCLUDE         := ./include
LIB		:= ./lib

LIBRARIES	:= -lglad -lglfw3dll
EXECUTABLE	:= main

all:./$(EXECUTABLE)

run: all
	./$(EXECUTABLE)

$(EXECUTABLE):$(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)
```

###### Finally open terminal in vscode run command "mingw32-make | .\main.exe" to build project and run program or(windows system) you can ./build.bat to build and run program

#### LearnOpengl with mingw makefile in vscode

[English](README.md) [Chinese](README_zh.md)

##### Build GLFW

- [Download](https://github.com/glfw/glfw/releases) 下载GLFW的源代码包，重新编译以确保能够在自己的机器上运行

- 解压源代码，打开CMake。
- 输入 source code 目录，以及 build 目录(自行创建)
- 点击 Configure，选择 MinGW Makefiles

- 完成后勾选 BUILD_SHARED_LIBS，创建动态库
- 点击 Generate

- 拷贝位于编译目录的src下的**glfw3.dll**、**libglfw3dll.a**
- 拷贝 source code 的 include

##### 配置GLAD

- [Download](https://glad.dav1d.de/) language - C/C++, API - 高于3.3, Profile - Core, Generate a loader, Generate, 下载压缩包
- gcc .\src\glad.c -c -I.\include\
- ar -rc libglad.a glad.o
- 拷贝**libglad.a** 和 include

###### 确保makefile格式正确，例如tab与空格

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

###### 最后在vscode终端mingw32-make | .\main.exe来编译，以及运行
#### LearnOpengl with mingw makefile in vscode

[English](README.md) [中文](README_zh.md)

##### 环境
- [Vscode](https://code.visualstudio.com/download) [MinGW](https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/) [Cmake](https://cmake.org/download/) 

##### Build GLFW

- [Download](https://github.com/glfw/glfw/releases) 下载GLFW的源代码包，重新编译以确保能够在自己的机器上运行

- 解压源代码，打开CMake。
- 输入 source code 目录，以及 build 目录(自行创建)
- 点击 Configure，选择 MinGW Makefiles

- 完成后勾选 BUILD_SHARED_LIBS，创建动态库
- 点击 Generate
- 进入build目录，$ minggw32-make ../
- 拷贝位于编译目录的src下的**glfw3.dll**、**libglfw3dll.a**
- 拷贝 source code 的 include

##### 配置GLAD

- [Download](https://glad.dav1d.de/) language - C/C++, API - 高于3.3, Profile - Core, Generate a loader, Generate, 下载压缩包
- gcc .\src\glad.c -c -I.\include\
- ar -rc libglad.a glad.o
- 拷贝**libglad.a** 和 include

###### Assimp

- [Download](https://github.com/assimp/assimp) Assimp源码并解压缩到本地目录
- 使用CMake对Assimp源码进行配置和生成构建文件，默认生成build目录
- 进入build目录，使用mingw32-make命令来编译Assimp源代码并生成可执行文件或库
- build\lib\libassimp.dll.a放到lib/目录下
- build\bin\libassimp-5.dll放到exe同级目录下

###### 加载模型

- assimp使用的是obj格式的模型，如果下载的是pmx等格式的模型，可以用blender导入模型(pmx可以用mmd_tools插件导入，[Download](https://github.com/powroupi/blender_mmd_tools))，再导出为obj格式文件，记得修改mtl文件的纹理文件名

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

###### 确保makefile格式正确，例如tab与空格

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

##### 最后在vscode终端mingw32-make | .\main.exe来编译，以及运行，或者(windows系统下) 你可以运行 ./build.bat 脚本来编译，以及运行程序

- 鼠标右键 (摄像机 | 光标)
- 滚轮(移动速度)
- W A S D(移动方向)
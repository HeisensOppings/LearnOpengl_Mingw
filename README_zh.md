#### [LearnOpengl](https://learnopengl.com/) with mingw makefile in vscode

[English](README.md) [中文](README_zh.md)

##### 环境
- Windows [Vscode](https://code.visualstudio.com/download) [MinGW](https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/)(x86_64-posix-seh) [Cmake](https://cmake.org/download/) 

##### 使用
```sh
cmake -S ./ -B ./build/ -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cd build
mingw32-make.exe -j4
opengl.exe
```

- 或者使用 build.bat

vscode终端 **mingw32-make | .\opengl.exe** 来编译，以及运行，或者直接 **.\build.bat**

- 鼠标右键 (摄像机 | 光标)
- 滚轮(移动速度)
- W A S D(移动方向)
- 左Ctrl键鼠标滚轮控制camera Fov
- 聚焦imgui窗口，以上操作失效

##### Build GLFW

- [GLFW](https://github.com/glfw/glfw/releases) 下载GLFW的源代码包，重新编译以确保能够在自己的机器上运行
- 解压源代码，打开CMake。
- 输入 source code 目录，以及 build 目录(自行创建)
- 点击 Configure，选择 MinGW Makefiles
- 完成后勾选 BUILD_SHARED_LIBS，创建动态库
- 点击 Generate
- 进入build目录，$ minggw32-make ../
- 拷贝位于编译目录的src下的**glfw3.dll**、**libglfw3dll.a**
- 拷贝 source code 的 include

##### 配置GLAD

- [GLAD](https://glad.dav1d.de/) language - C/C++, API - 高于3.3, Profile - Core, Generate a loader, Generate, 下载压缩包
- gcc .\src\glad.c -c -I.\include\
- ar -rc libglad.a glad.o
- 拷贝**libglad.a** 和 include

###### Assimp

- [Assimp](https://github.com/assimp/assimp) Assimp源码并解压缩到本地目录
- 使用CMake对Assimp源码进行配置和生成构建文件，默认生成build目录
- 进入build目录，使用mingw32-make命令来编译Assimp源代码并生成可执行文件或库
- build\lib\libassimp.dll.a放到lib/目录下
- build\bin\libassimp-5.dll放到exe同级目录下

###### 加载MMD模型

- 对于mmd模型例如pmx格式，assimp似乎只能加载tex纹理，也就是albedo/diffuse纹理，对应aiTextureType_DIFFUSE，并不能加载mmd模型的sph/spa toon纹理，但是我们可以转成obj模型，手动配置mtl文件来加载纹理，用blender导入(pmx可以用mmd_tools插件导入，[blender_mmd_tools](https://github.com/powroupi/blender_mmd_tools))，再导出为OBJ格式

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
...
```
```
    // gltf----------------------------obj---------------------------------name
    // aiTextureType_DIFFUSE           aiTextureType_DIFFUSE        map_Kd albedo      0      
    // aiTextureType_NORMALS           aiTextureType_NORMALS        map_Kn normal      1
    // aiTextureType_METALNESS         aiTextureType_SPECULAR       map_Ks metallic    2
    // aiTextureType_DIFFUSE_ROUGHNESS aiTextureType_SHININESS      map_Ns roughness   3
    // aiTextureType_LIGHTMAP          aiTextureType_AMBIENT        map_Ka ao          4
    // aiTextureType_EMISSIVE          aiTextureType_EMISSIVE       map_Ke emissive    5
```

###### [imgui](https://github.com/ocornut/imgui)

###### [FreeType](https://github.com/ubawurinna/freetype-windows-binaries)
- 使用CMake对FreeType源码进行配置和生成构建文件
- 进入build目录，使用mingw32-make编译并生成库
- 拷贝 source code 的 include
- build\lib\libfreetype.a放到lib/
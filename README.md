#### [LearnOpengl](https://learnopengl.com/) with mingw makefile in vscode

[English](README.md) [中文](README_zh.md)

##### Environment
- Windows [Vscode](https://code.visualstudio.com/download) [MinGW](https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/)(x86_64-posix-seh) [Cmake](https://cmake.org/download/) 

##### Use
```sh
cmake -S ./ -B ./build/ -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cd build
mingw32-make.exe -j4
opengl.exe
```

- or use **build.bat**

- Mouse right-click (Camera | Cursor)
- Scroll wheel (Movement speed)
- w a s d (Movement direction)
- Left Ctrl key mouse wheel controls camera Fov
- Focus on the imgui window, the above operation will not work

##### Build GLFW

- [GLFW](https://github.com/glfw/glfw/releases) the GLFW source code package and recompile it to make sure it runs on you own machine
- Unzip the source code and open CMake
- Input the source code and build(mkdir) directory
- Click Configure, Select MinGW Makefiles
- After Configure, Click BUILD_SHARED_LIBS, Create a dynamic library
- Click Generate
- In build directory run cmd $:mingw32-make ../
- Copy files in build/src/ **glfw3.dll**、**libglfw3dll.a**, and source code/include

##### Configure GLAD

- language - C/C++, API - higher than 3.3, Profile - Core, Generate a loader, Generate, [GLAD](https://glad.dav1d.de/) the zip
- gcc .\src\glad.c -c -I.\include\
- ar -rc libglad.a glad.o
- copy file **libglad.a** and include

###### Assimp

- [Assimp](https://github.com/assimp/assimp) Assimp source code and extract it to a local directory
- Use CMake to configure and generate build files for the Assimp source code, By default, the build directory will be generated
- Navigate to the build directory and use the mingw32-make command to compile the Assimp source code and generate executable files or libraries
- Put the build\lib\libassimp.dll.a file into lib/ directory
- Put the build\bin\libassimp-5.dll file into the same directory as the executable file(exe)

###### Loading MMD models

- For mmd models such as pmx format, it seems that assimp can only load tex textures(albedo/diffuse), corresponding to aiTextureType_DIFFUSE, and cannot load sph/spa toon textures of mmd models, but we can convert them to obj models and manually configure the mtl file load the texture, use blender to import the [blender_mmd_tools](https://github.com/powroupi/blender_mmd_tools), and then export it as an OBJ format file.

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
- Use CMake to configure the FreeType source code and generate build files.
- Enter the build directory and mingw32-make to compile and generate the libarary
- Copy the include of source code
- build\lib\libfreetype.a in the lib/

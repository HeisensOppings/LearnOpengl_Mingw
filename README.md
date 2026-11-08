#### [LearnOpengl](https://learnopengl.com/) with Qt6 (qmake)

[English](README.md) [中文](README_zh.md)

##### Environment (Qt6)
- Qt 6 with the widgets / opengl / openglwidgets modules, qmake
- Windows: use the Qt **MinGW 64-bit** kit. The bundled `lib/libassimp.dll.a` / `libassimp-5.dll` are MinGW builds and are not ABI compatible with an MSVC kit (build Assimp with MSVC if you need MSVC).
- Linux: system Assimp (e.g. `libassimp-dev`) or `qmake ASSIMP_PREFIX=/path/to/assimp`
- OpenGL 4.2 core profile required

##### Usage
```sh
mkdir build && cd build
qmake ../LearnOpengl.pro CONFIG+=release
mingw32-make -j4        # Linux: make -j4
release\opengl.exe     # Linux: ./opengl
# scenes: opengl [-cascaded | -animations]
```
Or open `LearnOpengl.pro` in Qt Creator. On Windows deploy the Qt runtime with windeployqt.

- Click the view to give it keyboard focus; right mouse button toggles camera look / cursor, Esc leaves look mode / closes
- Mouse wheel: movement speed; W A S D move, Space / Shift up / down
- Ctrl + wheel in look mode: camera FOV
- Qt control panel (replaces imgui): camera position/reset, fps, background colour, light, PBR, animation, CSM bias
- Cascaded scene: N cycles the debug layer, C freezes the light frustums
- The HDR environment background / skybox was removed: solid background colour, PBR ambient uses a constant-colour IBL
- GLFW, GLAD, FreeType (and all text rendering), imgui and stb_image were removed

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

#### [LearnOpengl](https://learnopengl.com/) with Qt6 (qmake)

[English](README.md) [中文](README_zh.md)

##### 环境 (Qt6)
- Qt 6（需要 widgets / opengl / openglwidgets 模块），qmake
- Windows：请使用 Qt 的 **MinGW 64-bit** 套件。仓库自带的 `lib/libassimp.dll.a`、`libassimp-5.dll` 是 MinGW 编译的，与 MSVC 套件 ABI 不兼容；使用 MSVC 时需要自行编译 MSVC 版 Assimp。
- Linux：系统安装 Assimp（如 `libassimp-dev`），或 `qmake ASSIMP_PREFIX=/path/to/assimp`
- 需要 OpenGL 4.2 Core

##### 使用
```sh
mkdir build && cd build
qmake ../LearnOpengl.pro CONFIG+=release
mingw32-make -j4        # Linux: make -j4
release\opengl.exe     # Linux: ./opengl
# 场景: opengl [-cascaded | -animations]
```
也可以直接用 Qt Creator 打开 `LearnOpengl.pro`。Windows 下需用 windeployqt 部署 Qt 运行库。

- 点击视图获得键盘焦点；鼠标右键切换（摄像机 | 光标），Esc 退出摄像机模式/关闭窗口
- 滚轮(移动速度)
- W A S D(移动方向)，Space / Shift 上升 / 下降
- 摄像机模式下 Ctrl + 滚轮控制 camera Fov
- 左侧 Qt 控制面板（替代原 imgui）：相机位置/重置、帧率、背景色、灯光、PBR、动画、CSM 参数
- Cascaded 场景：N 切换调试层，C 冻结光源视锥
- 原 HDR 环境背景 / 天空盒已移除，改为纯色背景；PBR 环境光使用常量颜色的 IBL
- 已移除 GLFW、GLAD、FreeType（及文字渲染）、imgui、stb_image

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

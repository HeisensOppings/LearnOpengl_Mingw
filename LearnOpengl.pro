# Qt6 / qmake build of LearnOpengl_Mingw
#   qmake LearnOpengl.pro && make          (Linux / mingw32-make on Windows)
# Requires Qt 6 with the opengl and openglwidgets modules and Assimp.

TEMPLATE = app
TARGET = opengl

QT += core gui widgets opengl openglwidgets
CONFIG += c++17 warn_on
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -Wextra -Wno-pragmas

INCLUDEPATH += $$PWD/include $$PWD/external
DEFINES += RES_DIR=\\\"$$PWD/res/\\\"

SOURCES += $$files($$PWD/src/*.cpp)
HEADERS += $$files($$PWD/include/*.h)

win32 {
    # Prebuilt MinGW import library shipped with the project (lib/libassimp.dll.a).
    # Only usable with a Qt "MinGW 64-bit" kit; an MSVC kit needs an MSVC build of Assimp.
    LIBS += -L$$PWD/lib -lassimp
    # copy the Assimp runtime next to the executable
    ASSIMP_DLL = $$shell_path($$PWD/lib/libassimp-5.dll)
    CONFIG(debug, debug|release): ASSIMP_DEST = $$shell_path($$OUT_PWD/debug)
    else: ASSIMP_DEST = $$shell_path($$OUT_PWD/release)
    QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$ASSIMP_DLL) $$quote($$ASSIMP_DEST) $$escape_expand(\\n\\t)
} else {
    # system Assimp (e.g. libassimp-dev); override with ASSIMP_PREFIX=/path on the qmake command line
    !isEmpty(ASSIMP_PREFIX) {
        INCLUDEPATH = $$PWD/include $$ASSIMP_PREFIX/include $$PWD/external
        LIBS += -L$$ASSIMP_PREFIX/lib
    }
    LIBS += -lassimp
}

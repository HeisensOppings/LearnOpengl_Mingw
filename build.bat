@echo off

SET SourceExe=opengl.exe

rem build.bat -c // clean
if "%1" == "-c" (
    if exist build (
        cmake -G Ninja -S . -B ./build
        cd build
        ninja clean
        cd ..
    )
)

rem Run CMake to generate the build files
if not exist build (
    cmake -G Ninja -S . -B ./build -DCMAKE_BUILD_TYPE=Release
    if errorlevel 1 (
        call :colorEcho 4 "Failed to create build directory"
        goto :eof
    )
)

rem compile exe and run it
cd build
ninja -j4
if not errorlevel 1 (
    if "%1" == "-c" (
        %SourceExe% "%2"
    ) else (
        %SourceExe% "%1"
    )
) else (
    call :colorEcho 4 "Compilation failed, please check the log"
)

goto :eof
:colorEcho
echo %~2
exit /b 0”
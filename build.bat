@echo off

SET SourceExe=opengl.exe

if "%1" == "rebuild" (
    @REM echo "Cleaning obj directory"
    @REM By default, .bat files are executed with Command Prompt.
    @REM PowerShell Get-Childitem -Path "obj" -files.
    @REM delims= is an option that specifies how for /f should split text lines.
    @REM The /b option only displays file names, excluding other information such as file size, modification data, etc.
    @REM The /a-d option indicates that inly files, not subdirectories, will be listed.

    if exist obj (
        for /f "delims=" %%f in ('dir /b /a-d obj') do (
            del "obj\%%f"
        )
    )
)

@REM if "%1" == "rebuild" (
@REM     echo "Cleaning obj and obj/imgui directories"
@REM     if exist obj rmdir /s /q obj
@REM     if exist obj\imgui rmdir /s /q obj\imgui
@REM )

@REM rem Remove the old executable
@REM if exist %SourceExe% del opengl.exe

rem Run mingw32-make to compile the program
mingw32-make -f Makefile
if not errorlevel 1 (
    rem Run the program if compilation succeeds
    opengl.exe
) else (
    rem Print error message if compilation fails
    call :colorEcho 4 "Compilation failed, please check the log"
)

goto :eof

:colorEcho
echo %~2
exit /b 0
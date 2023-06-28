@echo off

rem Remove the old executable
del main.exe

rem Run mingw32-make to compile the program
mingw32-make -f Makefile
if not errorlevel 1 (
    rem Run the program if compilation succeeds
    main.exe
) else (
    rem Print error message if compilation fails
    call :colorEcho 4 "Compilation failed, please check the log"
)

goto :eof

:colorEcho
echo %~2
exit /b 0
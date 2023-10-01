@echo off
cls
type BATCHLST.TXT
choice /C:1230 /N Please select what to build:
echo.
if ERRORLEVEL 4 goto end
if ERRORLEVEL 3 goto al_11
if ERRORLEVEL 2 goto al_109
if ERRORLEVEL 1 goto al950724

:al950724
set ASSREV=19950724
set DIR=AL950724
goto task
:al_109
set ASSREV=19960108
set DIR=AL_109
goto task
:al_11
set ASSREV=19960116
set DIR=AL_11
goto task

:task
md %DIR%
md %DIR%\OBJ
wmake.exe -f audio.mak "version = production" "libver_libdef = %DIR%" "libver_assrev = %ASSREV%"
:end

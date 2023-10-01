@echo off
cls
type BATCHLST.TXT
choice /C:1230 /N Please select what to clean:
echo.
if ERRORLEVEL 4 goto end
if ERRORLEVEL 1 set DIR=AL950724
if ERRORLEVEL 2 set DIR=AL_109
if ERRORLEVEL 3 set DIR=AL_11

del %DIR%\OBJ\*.*
del %DIR%\*.*
:end

@echo off 

set DEBUG_DIR=build-debug
set RELEASE_DIR=build-release

echo.
echo *******************************
echo *** Building ObjectiveFrame ***
echo *******************************
echo.

echo.
echo *** Configuring debug build
echo.
cmake -B %DEBUG_DIR% -DCMAKE_BUILD_TYPE=Debug --preset default
echo.
echo *** Building debug build
echo.
cmake --build %DEBUG_DIR% --config Debug -- /m
echo.
echo *** Configuring release build
echo.
cmake -B %RELEASE_DIR% -DCMAKE_BUILD_TYPE=Release --preset default
echo.
echo *** Building release build
echo.
cmake --build %RELEASE_DIR% --config Release -- /m





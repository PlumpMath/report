@echo off
rem Enter the script directory.
pushd %~dp0

rem Set common variables.
call third_party\common.bat

rem Determine the project name.
for /f "tokens=2 delims=( " %%i in ('findstr /c:"project(" CMakeLists.txt') do (
  set project=%%i
)

rem Create and enter the build directory.
if not exist build (
  mkdir build
)
pushd build

rem Generate and open the solution.
cmake -G "%generator%" -T "%toolset%" -DCMAKE_CONFIGURATION_TYPES="%configs%" -DCMAKE_INSTALL_PREFIX:PATH=.. ..
if %errorlevel% == 0 (
  start %project%.sln
) else (
  pause
)

rem Leave the build directory.
popd

rem Leave the script directory.
popd

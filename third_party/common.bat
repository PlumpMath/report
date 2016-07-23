@echo off
rem Set the CMake generator.
rem Options:
rem - Visual Studio 14 2015 Win64
rem - Visual Studio 14 2015
set generator=Visual Studio 14 2015 Win64

rem Set the CMake generator toolset.
rem Options:
rem - toolset=v140
rem - toolset=v140_xp
set toolset=v140

rem Set the CMake configurations.
rem Options:
rem - Release
rem - Release;Debug
set configs=Release;Debug

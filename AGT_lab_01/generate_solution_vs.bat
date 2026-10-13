@echo off
REM Generates engine.sln and the .vcxproj files for Visual Studio.
REM Double-click this, then open engine.sln.
REM
REM Uses the vs2022 project format, which Visual Studio 2026 opens directly.
REM The toolset (v145) is set in premake5.lua, not here.
call vendor\premake\premake5.exe vs2022
PAUSE

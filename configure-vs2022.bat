@echo off

REM A configuration helper that creates a Visual Studio 2022 Project.

cmake . -B "build_vs2022" -G"Visual Studio 17" -A x64 -DIGUANA_TEST=1

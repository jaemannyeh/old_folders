@echo off
call "C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin\vcvars32.bat"
goto %1
:build
devenv /build debug reason.sln
goto end
:run
.\build\debug\reason.exe
:end

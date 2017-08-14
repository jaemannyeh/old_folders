@echo off
call "C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin\vcvars32.bat"
goto %1
:build
devenv /build release reason.sln
goto end
:run
.\build\release\reason.exe
:end

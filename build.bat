@echo off

set compilerFlags=/nologo /MT /GR- /Gm- /EHsc /Od /Oi /FC /Zi /WX /W4 /wd4100 /I..\src\
set linkerFlags=/opt:ref /SUBSYSTEM:console /IGNORE:4204 ws2_32.lib

WHERE cl.exe >nul 2>nul
IF %ERRORLEVEL% NEQ 0 call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

IF NOT EXIST build\ mkdir build
pushd build

cl %compilerFlags% ..\src\server.cpp /link %linkerFlags%
cl %compilerFlags% ..\src\client.cpp /link %linkerFlags%

popd

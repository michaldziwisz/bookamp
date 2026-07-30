@echo off
REM Build dsp_bookamp.dll — x86, /MT, z GUI (.rc) i silnikami. Pelna wtyczka.
setlocal
set VS=C:\Program Files\Microsoft Visual Studio\2022\Community
call "%VS%\VC\Auxiliary\Build\vcvarsall.bat" x86 >nul
if errorlevel 1 (echo VCVARS_FAILED & exit /b 1)

cd /d D:\projekty\bookamp
if not exist build mkdir build

set ST=third_party\soundtouch
set BG=third_party\bungee
set LIBS=%ST%\build_x86\Release\SoundTouch.lib %BG%\build_x86\Release\bungee.lib %BG%\build_x86\Release\pffft.lib

rc /nologo /fo build\bookamp.res src\bookamp.rc
if errorlevel 1 (echo RC_FAILED & exit /b 1)

cl /nologo /LD /O2 /MT /EHsc /std:c++20 /W3 /D_USE_MATH_DEFINES ^
   /Isrc /I%ST%\include /I%BG% /I%BG%\submodules\eigen ^
   src\dsp_bookamp.cpp src\processor.cpp src\gui.cpp src\shortcuts.cpp src\settings.cpp ^
   src\enhance.cpp ^
   src\engine_soundtouch.cpp src\engine_bungee.cpp src\engine_factory.cpp ^
   build\bookamp.res ^
   /Fe:build\dsp_bookamp.dll /Fo:build\ ^
   /link /DEF:src\dsp_bookamp.def %LIBS% user32.lib gdi32.lib comctl32.lib oleacc.lib oleaut32.lib
if errorlevel 1 (echo BUILD_FAILED & exit /b 1)

echo BUILD_OK
dir /b build\dsp_bookamp.dll
endlocal

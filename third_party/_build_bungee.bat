@echo off
REM Build Bungee jako static lib, x86 (Win32), /MT. Tylko biblioteka.
setlocal
set CMAKE="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set SRC=D:\projekty\bookamp\third_party\bungee
set BUILD=D:\projekty\bookamp\third_party\bungee\build_x86

%CMAKE% -S "%SRC%" -B "%BUILD%" -G "Visual Studio 17 2022" -A Win32 -T v143 ^
  -DBUNGEE_BUILD_SHARED_LIBRARY=OFF ^
  -DCMAKE_C_FLAGS="/D_USE_MATH_DEFINES" -DCMAKE_CXX_FLAGS="/D_USE_MATH_DEFINES" ^
  -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded ^
  -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
if errorlevel 1 (echo CONFIGURE_FAILED & exit /b 1)

%CMAKE% --build "%BUILD%" --config Release --target bungee_library -j
if errorlevel 1 (echo BUILD_FAILED & exit /b 1)

echo BUILD_OK
dir /s /b "%BUILD%\*.lib"
endlocal

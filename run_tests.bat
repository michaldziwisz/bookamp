@echo off
REM Kanoniczna weryfikacja Bookamp: krzywa suwakow + silniki + rdzen Processor.
REM Bramka: jesli gdziekolwiek pojawi sie SOME_FAIL -> porazka.
setlocal
set LOG=%TEMP%\bookamp_tests.log
if exist "%LOG%" del "%LOG%"
REM skasuj stare artefakty, by nieudany build nie odpalil starego .exe (falszywy PASS)
del /q "D:\projekty\bookamp\build\test_mapping.exe" "D:\projekty\bookamp\build\test_settings.exe" "D:\projekty\bookamp\build\harness.exe" "D:\projekty\bookamp\build\test_processor.exe" 2>nul

call "D:\projekty\bookamp\_build_test_mapping.bat"   >> "%LOG%" 2>&1
call "D:\projekty\bookamp\_build_test_settings.bat"  >> "%LOG%" 2>&1
call "D:\projekty\bookamp\_build_harness.bat"        >> "%LOG%" 2>&1
call "D:\projekty\bookamp\_build_test_processor.bat" >> "%LOG%" 2>&1

echo ==== wyniki ====
findstr /R "RESULT: BUILD_FAILED VCVARS_FAILED PASS FAIL" "%LOG%"

findstr /C:"SOME_FAIL" /C:"BUILD_FAILED" /C:"VCVARS_FAILED" "%LOG%" >nul
if %errorlevel%==0 (echo TESTS_FAILED & exit /b 1)
echo TESTS_DONE
exit /b 0

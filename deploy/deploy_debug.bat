@echo off

set QT_DIR=D:\Qt-5.14.2\5.14.2\msvc2017
set PATH=%QT_DIR%\bin;%PATH%


rmdir %~dp0output-debug /S /Q
mkdir %~dp0output-debug

copy %~dp0..\out\QCefWidgetTest\Debug\QCefWidgetTest.exe %~dp0output-debug\QCefWidgetTest.exe


start "" /WAIT windeployqt.exe %~dp0output-debug\QCefWidgetTest.exe

xcopy %~dp019041_X86_DEBUG_DLL_FILES\* %~dp0output-debug\ /E /Y

xcopy %~dp0..\out\QCefWidget\bin\Debug\* %~dp0output-debug\ /E /Y

xcopy %~dp03rd\Debug\* %~dp0output-debug\ /E /Y

xcopy %~dp0..\test\TestResource\* %~dp0output-debug\TestResource\ /E /Y

pause
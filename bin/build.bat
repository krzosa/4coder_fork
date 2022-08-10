@echo off

set location=%cd%
set me="%~dp0"
cd %me%
cd ..
set src_root=%cd%
cd ..\build
set build_root=%cd%
set bin_root=%src_root%\bin
set custom_root=%src_root%\custom
set custom_bin=%custom_root\bin
cd %location%

set mode="-DDEV_BUILD"
rem if "%mode%" == "" (set mode="-DDEV_BUILD)

rem set f=-Wno-write-strings -Wno-comment -Wno-null-dereference -Wno-logical-op-parentheses -Wno-switch
rem set f=%f% -D_GNU_SOURCE -fPIC -fpermissive -g
rem set f=%f% %mode%
rem set f=%f% /I%src_root% /I%custom_root%

rem call clang %f% %bin_root%\4ed_build.cpp -lkernel32 -o build

set opts=/W4 /wd4310 /wd4100 /wd4201 /wd4505 /wd4996 /wd4127 /wd4510 /wd4512 /wd4610 /wd4390 /wd4189 /WX
set opts=%opts% /GR- /EHa- /nologo /FC /Zi
set opts=%opts% /I%src_root% /I%custom_root%
set opts=%opts% %mode%

set FirstError=0
pushd %build_root%
call cl %opts% %bin_root%\4ed_build.cpp /Febuild -link kernel32.lib
if %ERRORLEVEL% neq 0 (set FirstError=1)
if %ERRORLEVEL% neq 0 (goto END)
popd

%build_root%\build
:END
if %ERRORLEVEL% neq 0 (set FirstError=1)

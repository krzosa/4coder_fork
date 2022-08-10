@echo off

rem todo last flags should be fixed
set W=-Wno-writable-strings -Wno-deprecated-declarations -Wno-microsoft-goto -Wno-null-dereference -Wno-format
set L=-luser32.lib -lwinmm.lib -lgdi32.lib -lopengl32.lib -lcomdlg32.lib -luserenv.lib -l..\dependencies/x64/freetype.lib
set I=-I".." -I"..\custom" -I"..\dependencies/freetype2"
set N=-o 4ed
set F=-g -DOS_WINDOWS=1 -DOS_LINUX=0 -DOS_MAC=0

pushd %~dp0\..\custom
clang 4coder_default_bindings.cpp   %W% %I% %F% -DMETA_PASS -E -o 4coder_command_metadata.i
clang 4coder_metadata_generator.cpp %W% %I% %F% -o metadata_generator.exe
metadata_generator.exe -R "." ".\\4coder_command_metadata.i"

del 4coder_command_metadata.i
del metadata_generator.exe
popd

pushd %~dp0\..\ship_files
rem call ..\custom\bin\buildsuper_x64-win.bat
clang ..\custom\4coder_default_bindings.cpp %W% %I% %F% -shared -o custom_4coder.dll
clang ..\4ed_app_target.cpp %W% %I% %F% -shared %L% %N%_app.dll
clang ..\platform_win32/win32_4ed.cpp %W% %I% %F% %L% %N%.exe
popd


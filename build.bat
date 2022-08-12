@echo off

rem todo last flags should be fixed
set W=-Wno-writable-strings -Wno-deprecated-declarations
set L=-luser32.lib -lwinmm.lib -lgdi32.lib -lopengl32.lib -lcomdlg32.lib -luserenv.lib -ldependencies/x64/freetype.lib
set I=-I"dependencies/freetype2" -I"."
set N=-o 4ed
set F=-g -fno-exceptions -DOS_WINDOWS=1 -DOS_LINUX=0 -DOS_MAC=0

pushd %~dp0
rem clang 4ed_app_target.cpp %W% %I% %F% -DMETA_PASS -E -o 4coder_command_metadata.i
rem clang 4coder_metadata_generator.cpp %W% %I% %F% -o metadata_generator.exe
rem metadata_generator.exe -R "." ".\\4coder_command_metadata.i"

clang 4ed_app_target.cpp %W% %I% %F% -shared %L% %N%_app.dll
rem clang platform_win32/win32_4ed.cpp %W% %I% %F% %L% %N%.exe -Wno-microsoft-goto

copy 4ed.exe ship_files
copy 4ed_app.dll ship_files

popd



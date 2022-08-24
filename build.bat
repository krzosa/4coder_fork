@echo off

set W=-Wall -Wno-missing-braces -Wno-writable-strings -Wno-deprecated-declarations  -Wno-unused-function
set L=-luser32.lib -lwinmm.lib -lgdi32.lib -lopengl32.lib -lcomdlg32.lib -luserenv.lib -ldependencies/x64/freetype.lib
set I=-I"dependencies/freetype2" -I"."
set N=-o 4ed
set F=-g -fno-exceptions -fdiagnostics-absolute-paths -DOS_WINDOWS=1 -DOS_LINUX=0 -DOS_MAC=0
set R=-O2
rem -DSHIP_MODE


pushd %~dp0

rem LEXER GENERATOR
rem clang lexer_generator/4coder_cpp_lexer_gen.cpp %W% %I% %F% -o cpp_lexer.exe
rem cpp_lexer.exe

rem METAPROGRAM
clang 4ed_app_target.cpp %W% %I% %F% -DMETA_PASS -E -o 4coder_command_metadata.i
clang 4coder_metadata_generator.cpp %W% %I% %F% -o metadata_generator.exe
metadata_generator.exe -R "." ".\\4coder_command_metadata.i"

rem DLL AND EXE
clang 4ed_app_target.cpp %W% %I% %F% -shared %L% %N%_app.dll
clang platform_win32/win32_4ed.cpp %W% %I% %F% %L% %N%.exe -Wno-microsoft-goto


copy 4ed.exe ship_files
copy 4ed_app.dll ship_files
copy config.txt ship_files


copy 4ed.exe dev_version
copy 4ed_app.dll dev_version
copy config.txt dev_version

popd



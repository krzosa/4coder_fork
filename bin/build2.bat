@echo off
rem todo last flags should be fixed
set W=-Wno-writable-strings -Wno-deprecated-declarations -Wno-microsoft-goto -Wno-null-dereference -Wno-format
set L=-luser32.lib -lwinmm.lib -lgdi32.lib -lopengl32.lib -lcomdlg32.lib -luserenv.lib -ldependencies/x64/freetype.lib
set I=-I"." -I"custom" -I"dependencies/freetype2"
set N=-o 4ed
set F=-g

call custom/bin/buildsuper_x64-win.bat
clang 4ed_app_target.cpp %W% %I% %F% -shared %L% %N%_app.dll
clang platform_win32/win32_4ed.cpp %W% %I% %F% %L% %N%.exe

copy *.exe ship_files
copy *.pdb ship_files
copy *.dll ship_files

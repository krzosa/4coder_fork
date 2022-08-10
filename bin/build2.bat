
rem todo last flags should be fixed
set W=-Wno-writable-strings -Wno-deprecated-declarations -Wno-microsoft-goto -Wno-null-dereference -Wno-format
set L=-luser32.lib -lwinmm.lib -lgdi32.lib -lopengl32.lib -lcomdlg32.lib -luserenv.lib
set N=-o 4ed.exe
clang platform_win32/win32_4ed.cpp %W% -I"." -I"./custom" -I"dependencies/freetype2" -g  -ldependencies/x64/freetype.lib %L%

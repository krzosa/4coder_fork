cd "$(dirname "$0")"

W="-Wall -Wno-missing-braces -Wno-writable-strings -Wno-deprecated-declarations  -Wno-unused-function"
L="-lX11 -lpthread -lm -lrt -lGL -ldl -lXfixes -lfreetype -lfontconfig"
I="-Idependencies/freetype2 -I."
F="-g -fno-exceptions -fdiagnostics-absolute-paths -DOS_WINDOWS=0 -DOS_LINUX=1 -DOS_MAC=0"
R="-O2 -DSHIP_MODE"

# LEXER GENERATOR
clang lexer_generator/4coder_cpp_lexer_gen.cpp $W $I $F -o cpp_lexer.out
./cpp_lexer.out

# METAPROGRAM
clang 4ed_app_target.cpp $W $I $F -DMETA_PASS -E -o 4coder_command_metadata.i
clang 4coder_metadata_generator.cpp $W $I $F -o metadata_generator.out
./metadata_generator.out -R "." "./4coder_command_metadata.i"

# DLL AND EXE
clang 4ed_app_target.cpp -fPIC $W $I $F -shared $L -o 4ed_app.so
clang platform_linux/linux_4ed.cpp $W $I $F $L -o 4ed.out


cp 4ed.out ship_files
cp 4ed_app.so ship_files
cp config.txt ship_files


cp 4ed.out dev_version
cp 4ed_app.so dev_version
cp config.txt dev_version




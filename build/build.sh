set -xe
mkdir -p bin
CFLAGS="-lm -Wall -Wextra -Wpedantic -I./headers" 
CLIBS="-L./lib/lib_linux -lraylib"
gcc $CFLAGS -o ./bin/main ./src/main.c $CLIBS

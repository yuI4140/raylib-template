set -xe
CFLAGS="-lm -Wall -Wextra -I./headers" 
CLIBS="-L./lib/lib_linux -lraylib"
gcc $CFLAGS -o bin/main src/main.c $CLIBS

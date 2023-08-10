set -xe
CFLAGS="-lm -Wall -Wextra -I./headers"
CLIBS="-L./lib -lraylib"
gcc $CFLAGS -o bin/main src/main.c $CLIBS

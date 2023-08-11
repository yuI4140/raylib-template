set -xe
CFLAGS="-lm -Wall -Wextra -Werror -Wpedantic `pkg-config --cflags raylib`"
CLIBS="`pkg-config --libs raylib`"
gcc $CFLAGS -o bin/main src/main.c $CLIBS

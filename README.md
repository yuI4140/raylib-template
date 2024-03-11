# raylib-template
A template for raylib
## Platfom support
- linux
- windows
## building
```bash
cd ./lib/raylib_latest/ 
```
Now you can choose between:
- `make PLATFORM=PLATFORM_DESKTOP` # To make the static version.
- `make PLATFORM=PLATFORM_DESKTOP` RAYLIB_LIBTYPE=SHARED` # To make the dynamic shared version.
run:
```bash
C-COMPILER -o nob nob.c # Only Once
./nob
```
And now you have the template set up.

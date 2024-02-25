#define RAYLIB_IMP
#include "../headers/raylib_layer.h"
int main() {
  InitWindow(800, 600, "Win");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    DrawFPS(0, 0);
    BeginDrawing();
    ClearBackground(to_hex("#181818"));
    EndDrawing();
  }
  CloseWindow();
  return 0;
}

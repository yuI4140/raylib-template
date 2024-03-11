#include "../headers/raylib.h"
int main() {
  InitWindow(800, 600, "Win");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    DrawFPS(0, 0);
    BeginDrawing();
    ClearBackground(GetColor(0x181818ff));
    EndDrawing();
  }
  CloseWindow();
  return 0;
}

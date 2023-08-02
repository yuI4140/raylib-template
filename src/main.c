#define RAYLIB_IMP
#include "../headers/raylib_layer.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    Win* w=initWin(newVec2(800,600),"Win");
    InitWindow(w->area.x,w->area.y,w->title); 
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
    DrawFPS(0,0); 
    BeginDrawing(); 
    ClearBackground(to_hex("#181818"));
    EndDrawing();
    }
    CloseWindow();

    return 0;
}

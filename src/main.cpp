#include <iostream>
#include <cstdlib>
#include <raylib.h>
#include "board.h"
#include "game.h"
#include "home.h"
#include "constants.h"

using namespace std;

int main()
{
    srand(time(NULL));
    InitAudioDevice();
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chess Game");
    SetExitKey(0);
    SetTargetFPS(60);
    HomeScreen hs;
    while(!WindowShouldClose() && hs.quit == false)
    {
        BeginDrawing();
        ClearBackground(GRAY);
        hs.Run();
        DrawRectangleLinesEx(Rectangle{(float)OFFSET - 6, (float)OFFSET - 6, (float)CELL_SIZE * 8 + 12, (float)CELL_SIZE * 8 + 12}, 6, BLACK);
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
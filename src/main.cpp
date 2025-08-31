#include <iostream>
#include<cstdlib>
#include <raylib.h>
#include "board.h"
#include "game.h"
using namespace std;

int main()
{
    srand(time(NULL));
    int cellSize = 60;
    int cellCount = 8;
    int offset = 30; 
    InitAudioDevice();
    InitWindow(cellSize* cellCount + 2 * offset, cellCount*cellSize + 2 * offset, "Chess Game");
    SetTargetFPS(60);
    Game game(cellSize, offset);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GRAY);
        game.Run();
        DrawRectangleLinesEx(Rectangle{(float)offset - 6, (float)offset - 6, (float)cellSize*8 + 12, (float)cellSize*8 + 12}, 6, BLACK);
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
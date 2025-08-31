#pragma once
#include "board.h"
class Game
{
public:
    Game(int cellSize, int offset);
    void Run();
    void HandleGameOver();
    void DrawGameOverScreen();
    void ResetGame();
    int cellSize;
    int offset;
    std::string winner;
private:
    Board board;
};
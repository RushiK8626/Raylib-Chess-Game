#pragma once
#include "board.h"
class Game
{
public:
    Game(int cellSize, int offset);
    void Run();
    void HandleGameOver(bool isCheckmate);
    void DrawGameOverScreen(std::string &winner);
    void ResetGame();
    int cellSize;
    int offset;
private:
    Board board;
};
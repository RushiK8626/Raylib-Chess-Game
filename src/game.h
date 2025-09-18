#pragma once
#include "board.h"
#include "home.h"

class HomeScreen; 

class Game
{
public:
    Game(HomeScreen::Mode mode);
    void Run();
    void HandleGameOver();
    void DrawGameOverScreen();
    void ResetGame();
    bool matchRunning;
    std::string winner;

private:
    Board board;
    Font myFont;
};
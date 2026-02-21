#pragma once
#include "board.h"
#include "home.h"

class HomeScreen; 

class Game
{
    std::string winner;
    Font myFont;
    Board board;
    
public:
    bool matchRunning;

    Game(HomeScreen::Mode mode);
    void Run();
    void HandleGameOver();
    void DrawGameOverScreen();
    void ResetGame();
};
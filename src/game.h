#pragma once
#include "board.h"
#include "home.h"
#include <memory>

class HomeScreen; 

class Game
{
    std::string winner;
    Font myFont;
    std::unique_ptr<Board> board;
    HomeScreen::Mode gameMode;
    
public:
    Game(HomeScreen::Mode mode);
    
    bool matchRunning;

    void Run();
    void HandleGameOver();
    void DrawGameOverScreen();
    void ResetGame();
};
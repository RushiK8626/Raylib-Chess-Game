#pragma once
#include "board.h"
#include "home.h"
#include <memory>

class HomeScreen; 

class Game
{
    bool paused;
    std::string winner;
    Font myFont;
    Font fontBold;
    Font titleFont;
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
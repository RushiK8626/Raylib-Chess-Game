#include "game.h"

Game::Game(HomeScreen::Mode mode)
    : board(mode), winner("NA"), matchRunning(true)
{
    myFont = LoadFontEx("font/firasans.ttf", 32, 0, 0);
}

void Game::Run()
{

    if (!board.gameOver)
    {
        board.Update();
    }
    board.Draw();
    if (board.gameOver)
    {
        HandleGameOver();
    }
}


void Game::HandleGameOver()
{
    if (board.victory)
    {
        winner = board.isWhiteMov ? "Black" : "White";
        // Disable further moves
        board.gameOver = true;
        board.victory = true;
        DrawGameOverScreen();
    }
    else if(board.draw)
    {
        winner = "NA";
        DrawGameOverScreen();
    }
    else board.gameOver = false;
}

void Game::DrawGameOverScreen()
{
    // Draw an opaque background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    if(winner != "NA")
    {
        Vector2 textSize = MeasureTextEx(myFont, "CHECKMATE", 50, 2);
        Vector2 pos = {
            (GetScreenWidth() - textSize.x) / 2.0f, 
            (GetScreenHeight() / 2.0f) - 80.0f
        };
        DrawTextEx(myFont, "CHECKMATE", pos, 50, 2, RED);

        std::string winnerText = winner + " Wins!";
        Vector2 winnerSize = MeasureTextEx(myFont, winnerText.c_str(), 30, 2);
        Vector2 winnerPos = {
            (GetScreenWidth() - winnerSize.x) / 2.0f,
            (GetScreenHeight() / 2.0f) - 20.0f
        };
        DrawTextEx(myFont, winnerText.c_str(), winnerPos, 30, 2, WHITE);

    }
    else if (winner == "NA")
    {
        Vector2 drawSize = MeasureTextEx(myFont, "DRAW", 50, 2);
        Vector2 drawPos = {
            (GetScreenWidth() - drawSize.x) / 2.0f,
            (GetScreenHeight() / 2.0f) - 80.0f
        };
        DrawTextEx(myFont, "DRAW", drawPos, 50, 2, RED);
    }

    Rectangle btnRectHome = {
        (float)GetScreenWidth() / 2 - 60,
        (float)GetScreenHeight() / 2 + 150,
        120,
        50};

    DrawRectangleRounded(btnRectHome, 0.3f, 10, LIGHTGRAY);
    DrawRectangleLinesEx(btnRectHome, 2, DARKGRAY);
    Vector2 homeButtonSize = MeasureTextEx(myFont, "HOME", 25, 2);
    Vector2 resetPos = {
        btnRectHome.x + (btnRectHome.width - homeButtonSize.x) / 2.0f,
        btnRectHome.y + 10.0f
    };
    DrawTextEx(myFont, "HOME", resetPos, 25, 2, BLACK);

    Rectangle btnRectReset = {
        (float)GetScreenWidth() / 2 - 60,
        (float)GetScreenHeight() / 2 + 80,
        120,
        50};

    DrawRectangleRounded(btnRectReset, 0.3f, 10, LIGHTGRAY);
    DrawRectangleLinesEx(btnRectReset, 2, DARKGRAY);
    Vector2 resetSize = MeasureTextEx(myFont, "RESET", 25, 2);
    Vector2 homeButtonPos = {
        btnRectReset.x + (btnRectReset.width - resetSize.x) / 2.0f,
        btnRectReset.y + 10.0f
    };
    DrawTextEx(myFont, "RESET", homeButtonPos, 25, 2, BLACK);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnRectReset))
    {
        ResetGame();
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnRectHome))
    {
        matchRunning = false;
    }
}

void Game::ResetGame()
{
    // Reset board to initial state
    board.isWhiteMov = true;
    board.selectedPiece = nullptr;
    board.dragging = false;
    board.gameOver = false;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            board.board[row][col] = Piece();
        }
    }
    board.InitializePieces();
    // Clear move history
    while (!board.moveHistory.empty())
    {
        board.moveHistory.pop_back();
    }
    board.gameOver = false;
}
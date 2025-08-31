#include "game.h"
#include "board.h"

Game::Game(int cellSize, int offset)
    : board(cellSize, offset) 
{
    Board board(cellSize, offset);
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
        HandleGameOver(true);
    }
}


void Game::HandleGameOver(bool isCheckmate)
{
    if (isCheckmate)
    {
        std::string winner = board.isWhiteMov ? "Black" : "White";
        // Disable further moves
        board.gameOver = true;

        DrawGameOverScreen(winner);
    }
    else board.gameOver = false;
}

void Game::DrawGameOverScreen(std::string &winner)
{
    // Draw an opaque background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    int textWidth = MeasureText("CHECKMATE", 50);
    DrawText("CHECKMATE", (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2 - 80, 50, RED);

    std::string winnerText = winner + " Wins!";
    textWidth = MeasureText(winnerText.c_str(), 30);
    DrawText(winnerText.c_str(), (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2 - 20, 30, WHITE);

    Rectangle btnRect = {
        (float)GetScreenWidth() / 2 - 60,
        (float)GetScreenHeight() / 2 + 80,
        120,
        50};

    DrawRectangleRounded(btnRect, 0.3f, 10, LIGHTGRAY);
    DrawRectangleLinesEx(btnRect, 2, DARKGRAY);

    textWidth = MeasureText("RESET", 25);
    DrawText("RESET", btnRect.x + (btnRect.width - textWidth) / 2, btnRect.y + 10, 25, BLACK);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnRect))
    {
        ResetGame();
    }
}


void Game::ResetGame()
{
    // Reset board to initial state
    board.isWhiteMov = false;
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
    while (!board.boardHistory.empty())
    {
        board.boardHistory.pop();
    }
    board.gameOver = false;
}
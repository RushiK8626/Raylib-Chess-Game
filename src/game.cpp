#include "game.h"
#include "buttons.h"

Game::Game(HomeScreen::Mode mode)
    : paused(false), winner("NA"), board(std::make_unique<Board>(mode)), gameMode(mode), matchRunning(true)
{
    myFont = LoadFontEx("font/Lora-Italic.ttf", 32, 0, 0);
    fontBold = LoadFontEx("font/Lora-BoldItalic.ttf", 32, 0, 0);
    titleFont = LoadFontEx("font/LuckiestGuy.ttf", 90, 0, 0);

    if (myFont.texture.id == 0)
    {
        myFont = GetFontDefault();
    }
    if (fontBold.texture.id == 0)
    {
        fontBold = GetFontDefault();
    }
    if (titleFont.texture.id == 0)
    {
        titleFont = GetFontDefault();
    }
}

void Game::Run()
{
    if (board->engineFailed)
    {
        TraceLog(LOG_WARNING, "Stockfish engine failed to load. Returning to home screen.");
        matchRunning = false;
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        paused = !paused;
    }

    if (!paused && !board->gameOver)
    {
        board->Update();
    }

    board->Draw();

    if (board->gameOver || paused)
    {
        HandleGameOver();
    }
}

void Game::HandleGameOver()
{
    if (board->victory)
    {
        winner = board->isWhiteMov ? "Black" : "White";
        // Disable further moves
        board->gameOver = true;
        board->victory = true;
        DrawGameOverScreen();
    }
    else if (board->draw)
    {
        winner = "NA";
        DrawGameOverScreen();
    }
    else if (paused)
    {
        DrawGameOverScreen();
    }
    else
        board->gameOver = false;
}

void Game::DrawGameOverScreen()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.75f));

    float centerX = GetScreenWidth() / 2.0f - 60;

    Rectangle btnHome = {centerX, (float)GetScreenHeight() / 2 - 100, 120, 50};
    Rectangle btnReset = {centerX, (float)GetScreenHeight() / 2 - 30, 120, 50};
    Rectangle btnContinue = {centerX, (float)GetScreenHeight() / 2 + 40, 120, 50};

    if (paused)
    {
        // HOME button - cool blue (clean, neutral action)
        DrawGradientButton(btnHome, "HOME", myFont, fontBold,
                           Color{70, 130, 200, 220}, // base
                           Color{120, 180, 255, 255} // hover
        );

        // RESET button - warm red (destructive action)
        DrawGradientButton(btnReset, "RESET", myFont, fontBold,
                           Color{180, 70, 70, 220},
                           Color{255, 100, 100, 255});

        // CONTINUE button - vibrant green (positive action)
        DrawGradientButton(btnContinue, "CONTINUE", myFont, fontBold,
                           Color{60, 150, 90, 220},
                           Color{100, 220, 140, 255});

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, btnReset))
            {
                ResetGame();
                paused = false;
            }
            else if (CheckCollisionPointRec(mouse, btnHome))
            {
                matchRunning = false;
            }
            else if (CheckCollisionPointRec(mouse, btnContinue))
            {
                paused = false;
            }
        }
    }
    else
    {
        std::string mainText = (winner != "NA") ? "CHECKMATE" : "DRAW";

        float fontSize = 72;
        float spacing = 5;

        Vector2 size = MeasureTextEx(titleFont, mainText.c_str(), fontSize, spacing);

        Vector2 pos = {
            (GetScreenWidth() - size.x) / 2.0f,
            (float)GetScreenHeight() / 2 - 140};

        // Shadow
        DrawTextEx(titleFont, mainText.c_str(),
                   Vector2{pos.x + 3.0f, pos.y + 3.0f},
                   fontSize, spacing,
                   Color{0, 0, 0, 180});

        DrawTextEx(titleFont, mainText.c_str(),
                   pos,
                   fontSize, spacing,
                   Color{255, 235, 59, 255});

        if (winner != "NA")
        {
            std::string text = winner + " Wins!";

            float subSize = 40;
            Vector2 wsize = MeasureTextEx(myFont, text.c_str(), subSize, 2);

            Vector2 wpos = {
                (GetScreenWidth() - wsize.x) / 2.0f,
                (float)GetScreenHeight() / 2 - 60};

            // Shadow
            DrawTextEx(myFont, text.c_str(),
                       {wpos.x + 2, wpos.y + 2},
                       subSize, 2,
                       Color{0, 0, 0, 180});

            DrawTextEx(myFont, text.c_str(),
                       wpos,
                       subSize, 2,
                       Color{230, 230, 230, 255});
        }

        Rectangle btnHome2 = {centerX, (float)GetScreenHeight() / 2 + 80, 120, 50};
        Rectangle btnReset2 = {centerX, (float)GetScreenHeight() / 2 + 150, 120, 50};

        DrawGradientButton(btnHome2, "HOME", myFont, fontBold,
                           Color{70, 130, 200, 220},
                           Color{120, 180, 255, 255});

        DrawGradientButton(btnReset2, "RESET", myFont, fontBold,
                           Color{180, 70, 70, 220},
                           Color{255, 100, 100, 255});

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, btnReset2))
            {
                ResetGame();
            }
            else if (CheckCollisionPointRec(mouse, btnHome2))
            {
                matchRunning = false;
            }
        }
    }
}

void Game::ResetGame()
{
    // Delete previous board and create a fresh one with same mode
    board = std::make_unique<Board>(gameMode);
    winner = "NA";
}
#include "home.h"
#include "game.h"
#include "buttons.h"
#include "constants.h"
#include <raylib.h>
#include <string>
#include <cmath>

HomeScreen::HomeScreen()
{
	font = LoadFontEx("font/Lora-italic.ttf", 50, 0, 0);
	hoverFont = LoadFontEx("font/Lora-BoldItalic.ttf", 50, 0, 0);
	titleFont = LoadFontEx("font/LuckiestGuy.ttf", 90, 0, 0);
	if (font.texture.id == 0)
	{
		font = GetFontDefault();
	}
	if (hoverFont.texture.id == 0)
	{
		hoverFont = GetFontDefault();
	}
	if (titleFont.texture.id == 0)
	{
		titleFont = GetFontDefault();
	}
	layoutButtons();
	animationTimer = 0.0f;
}

HomeScreen::~HomeScreen()
{
	if (font.texture.id != 0 && font.texture.id != GetFontDefault().texture.id)
		UnloadFont(font);
	if (titleFont.texture.id != 0 && titleFont.texture.id != GetFontDefault().texture.id)
		UnloadFont(titleFont);
}

void HomeScreen::Run()
{
	animationTimer += GetFrameTime();
	// If a game has started, run the game loop instead of menu
	if (startGame && game)
	{
		game->Run();
		// If the game has ended, reset game pointer so a new game can be started
		if (!game->matchRunning)
		{
			game.reset();
			startGame = false;
			selectedMode = NONE;
			confirmScreen = false;
		}
		return;
	}
	Draw();
	Update();
}

void HomeScreen::Update()
{
	if (startGame || quit)
		return;

	if (!confirmScreen)
	{
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			Vector2 mp = GetMousePosition();
			if (CheckCollisionPointRec(mp, btnEngineWhite))
			{
				selectedMode = VS_ENGINE_WHITE;
				confirmScreen = true;
			}
			else if (CheckCollisionPointRec(mp, btnEngineBlack))
			{
				selectedMode = VS_ENGINE_BLACK;
				confirmScreen = true;
			}
			else if (CheckCollisionPointRec(mp, btnHuman))
			{
				selectedMode = HUMAN_VS_HUMAN;
				confirmScreen = true;
			}
			else if (CheckCollisionPointRec(mp, btnQuit))
			{
				quit = true;
			}
		}
	}
	else
	{ // confirm screen
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			Vector2 mp = GetMousePosition();
			if (CheckCollisionPointRec(mp, btnStart))
			{
				// create a new game instance
				game = std::make_unique<Game>(selectedMode);
				startGame = true;
			}
			else if (CheckCollisionPointRec(mp, btnBack))
			{
				selectedMode = NONE;
				confirmScreen = false;
			}
		}
		if (IsKeyPressed(KEY_ESCAPE))
		{
			selectedMode = NONE;
			startGame = false;
			confirmScreen = false;
		}
	}
}

void HomeScreen::Reset()
{
	startGame = false;
	quit = false;
	selectedMode = NONE;
	confirmScreen = false;
	animationTimer = 0.0f;
	if (game)
		game->ResetGame();
}

void HomeScreen::Draw() const
{
	// Animated background smooth animated gradient
	float t = animationTimer * 0.3f;

	Color bg1 = {
		(unsigned char)(20 + 10 * sinf(t)),
		(unsigned char)(30 + 15 * sinf(t + 1.0f)),
		(unsigned char)(60 + 20 * sinf(t + 2.0f)),
		255};

	Color bg2 = {
		(unsigned char)(60 + 20 * sinf(t + 2.5f)),
		(unsigned char)(20 + 10 * sinf(t + 0.5f)),
		(unsigned char)(80 + 25 * sinf(t + 1.5f)),
		255};

	DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, bg1, bg2);

	// Title with shadow
	std::string title = "CHESS GAME";
	float titleFontSize = 72;
	float titleSpacing = 5;

	Vector2 tsize = MeasureTextEx(titleFont, title.c_str(), titleFontSize, titleSpacing);
	Vector2 titlePos = {(WINDOW_WIDTH - tsize.x) / 2.0f, 80.0f};

	// Shadow (draw first, slightly offset, darker color)
	DrawTextEx(titleFont, title.c_str(),
			   {titlePos.x + 3.0f, titlePos.y + 3.0f},
			   titleFontSize, titleSpacing,
			   Color{0, 0, 0, 180});

	// Main title
	DrawTextEx(titleFont, title.c_str(),
			   titlePos,
			   titleFontSize, titleSpacing,
			   Color{255, 235, 59, 255});

	if (!confirmScreen)
	{
		// HOME SCREEN BUTTONS (with better colors + shadow)

		DrawButtonShadow(btnEngineWhite);
		DrawGradientButton(btnEngineWhite, "Play vs Engine (White)", font, hoverFont,
						   Color{70, 130, 200, 220}, // calm blue
						   Color{120, 180, 255, 255});

		DrawButtonShadow(btnEngineBlack);
		DrawGradientButton(btnEngineBlack, "Play vs Engine (Black)", font, hoverFont,
						   Color{60, 60, 70, 220}, // darker neutral
						   Color{100, 100, 120, 255});

		DrawButtonShadow(btnHuman);
		DrawGradientButton(btnHuman, "Human vs Human", font, hoverFont,
						   Color{60, 150, 90, 220}, // green
						   Color{100, 220, 140, 255});

		DrawButtonShadow(btnQuit);
		DrawGradientButton(btnQuit, "Quit", font, hoverFont,
						   Color{170, 70, 70, 220}, // red
						   Color{255, 110, 110, 255});

		float hintSize = 26;
		Vector2 hint = MeasureTextEx(font, "Choose your game mode", hintSize, 2);

		DrawTextEx(font, "Choose your game mode",
				   {(WINDOW_WIDTH - hint.x) / 2.0f, btnQuit.y + btnQuit.height + 35},
				   hintSize, 2, Color{200, 200, 200, 200});
	}
	else
	{
		std::string modeText;

		switch (selectedMode)
		{
		case VS_ENGINE_WHITE:
			modeText = "Engine Game: You are White";
			break;
		case VS_ENGINE_BLACK:
			modeText = "Engine Game: You are Black";
			break;
		case HUMAN_VS_HUMAN:
			modeText = "Human vs Human";
			break;
		default:
			break;
		}

		float modeSize = 34; // increased from 28
		Vector2 ms = MeasureTextEx(font, modeText.c_str(), modeSize, 2);

		DrawTextEx(font, modeText.c_str(),
				   {(WINDOW_WIDTH - ms.x) / 2.0f, btnStart.y - 90},
				   modeSize, 2, WHITE);

		DrawButtonShadow(btnStart);
		DrawGradientButton(btnStart, "Start Game", font, hoverFont,
						   Color{50, 150, 50, 220}, Color{80, 180, 80, 255});

		DrawButtonShadow(btnBack);
		DrawGradientButton(btnBack, "Back", font, hoverFont,
						   Color{150, 150, 50, 220}, Color{180, 180, 80, 255});

		float escSize = 22; // increased from 18
		Vector2 esc = MeasureTextEx(font, "Press ESC to go back", escSize, 2);

		DrawTextEx(font, "Press ESC to go back",
				   {(WINDOW_WIDTH - esc.x) / 2.0f, btnBack.y + btnBack.height + 30},
				   escSize, 2, Color{180, 180, 180, 200});
	}

	// Border
	DrawRectangleLines(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Color{100, 100, 100, 100});
}

void HomeScreen::layoutButtons()
{
	const int bw = 320;
	const int bh = 70;
	const int gap = 30;
	int totalH = 4 * bh + 3 * gap;
	int startY = (WINDOW_HEIGHT - totalH) / 2 + 50; // Offset down a bit for title
	int cx = WINDOW_WIDTH / 2 - bw / 2;

	btnEngineWhite = {(float)cx, (float)startY + 0 * (bh + gap), (float)bw, (float)bh};
	btnEngineBlack = {(float)cx, (float)startY + 1 * (bh + gap), (float)bw, (float)bh};
	btnHuman = {(float)cx, (float)startY + 2 * (bh + gap), (float)bw, (float)bh};
	btnQuit = {(float)cx, (float)startY + 3 * (bh + gap), (float)bw, (float)bh};

	// confirmation buttons - larger and more prominent
	const int confirmBw = 220;
	const int confirmBh = 70;
	const int confirmGap = 50;
	int confirmStartY = WINDOW_HEIGHT / 2 - confirmBh;
	int confirmCx = WINDOW_WIDTH / 2 - confirmBw / 2;

	btnStart = {(float)confirmCx, (float)confirmStartY - confirmGap, (float)confirmBw, (float)confirmBh};
	btnBack = {(float)confirmCx, (float)confirmStartY + confirmGap, (float)confirmBw, (float)confirmBh};
}

bool HomeScreen::hover(const Rectangle &r) const
{
	return CheckCollisionPointRec(GetMousePosition(), r);
}

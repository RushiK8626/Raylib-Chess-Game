#include "home.h"
#include "game.h" 
#include "constants.h"
#include <raylib.h>
#include <string>
#include <cmath>

HomeScreen::HomeScreen() 
{
	font = LoadFontEx("font/firasans.ttf", 32, 0, 0);
	titleFont = LoadFontEx("font/NotoSansSymbols.ttf", 72, 0, 0);
	if (font.texture.id == 0) {
		font = GetFontDefault(); // fallback
	}
	if (titleFont.texture.id == 0) {
		titleFont = GetFontDefault(); // fallback
	}
	layoutButtons();
	animationTimer = 0.0f;
}

HomeScreen::~HomeScreen() {
	if (font.texture.id != 0 && font.texture.id != GetFontDefault().texture.id) UnloadFont(font);
	if (titleFont.texture.id != 0 && titleFont.texture.id != GetFontDefault().texture.id) UnloadFont(titleFont);
}

void HomeScreen::Run() 
{
    animationTimer += GetFrameTime();
    // If a game has started, run the game loop instead of menu
    if(startGame && game) {
        game->Run();
        // If the game has ended, reset game pointer so a new game can be started
        if (!game->matchRunning) {
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

void HomeScreen::Reset() {
	startGame = false;
	quit = false;
	selectedMode = NONE;
	confirmScreen = false;
	animationTimer = 0.0f;
	if(game) game->ResetGame();
}

void HomeScreen::layoutButtons() {
	const int bw = 320;
	const int bh = 70;
	const int gap = 20;
	int totalH = 4 * bh + 3 * gap;
	int startY = (WINDOW_HEIGHT - totalH) / 2 + 50; // Offset down a bit for title
	int cx = WINDOW_WIDTH / 2 - bw / 2;

	btnEngineWhite = { (float)cx, (float)startY + 0 * (bh + gap), (float)bw, (float)bh };
	btnEngineBlack = { (float)cx, (float)startY + 1 * (bh + gap), (float)bw, (float)bh };
	btnHuman       = { (float)cx, (float)startY + 2 * (bh + gap), (float)bw, (float)bh };
	btnQuit        = { (float)cx, (float)startY + 3 * (bh + gap), (float)bw, (float)bh };

	// confirmation buttons - larger and more prominent
	const int confirmBw = 200;
	const int confirmGap = 30;
	int confirmStartY = WINDOW_HEIGHT / 2 - bh;
	int confirmCx = WINDOW_WIDTH / 2 - confirmBw / 2;

	btnStart = { (float)confirmCx, (float)confirmStartY - confirmGap, (float)confirmBw, (float)bh };
	btnBack  = { (float)confirmCx, (float)confirmStartY + confirmGap, (float)confirmBw, (float)bh };
}

bool HomeScreen::hover(const Rectangle& r) const {
	return CheckCollisionPointRec(GetMousePosition(), r);
}

// void HomeScreen::Update() {
// 	if (startGame || quit) return; // nothing else

// 	if (!confirmScreen) {
// 		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
// 			Vector2 mp = GetMousePosition();
// 			if (CheckCollisionPointRec(mp, btnEngineWhite)) { 
// 				selectedMode = VS_ENGINE_WHITE; 
// 				confirmScreen = true; 
// 			}
// 			else if (CheckCollisionPointRec(mp, btnEngineBlack)) { 
// 				selectedMode = VS_ENGINE_BLACK; 
// 				confirmScreen = true; 
// 			}
// 			else if (CheckCollisionPointRec(mp, btnHuman)) { 
// 				selectedMode = HUMAN_VS_HUMAN; 
// 				confirmScreen = true; 
// 			}
// 			else if (CheckCollisionPointRec(mp, btnQuit)) { 
// 				quit = true; 
// 			}
// 		}
// 	} else { // confirm screen
// 		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
// 			Vector2 mp = GetMousePosition();
// 			if (CheckCollisionPointRec(mp, btnStart)) {
// 				// Create game instance with selected mode (only once)
// 				if(!game) game = std::make_unique<Game>(cellSize, offset, selectedMode);
// 				startGame = true;
// 			} else if (CheckCollisionPointRec(mp, btnBack)) {
// 				selectedMode = NONE;
// 				confirmScreen = false;
// 			}
// 		}
// 		if (IsKeyPressed(KEY_ESCAPE)) {
// 			selectedMode = NONE;
// 			confirmScreen = false;
// 		}
// 	}
// }

void HomeScreen::Update() {
    if (startGame || quit) return; // nothing else

    if (!confirmScreen) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, btnEngineWhite)) { 
                selectedMode = VS_ENGINE_WHITE; 
                confirmScreen = true; 
            }
            else if (CheckCollisionPointRec(mp, btnEngineBlack)) { 
                selectedMode = VS_ENGINE_BLACK; 
                confirmScreen = true; 
            }
            else if (CheckCollisionPointRec(mp, btnHuman)) { 
                selectedMode = HUMAN_VS_HUMAN; 
                confirmScreen = true; 
            }
            else if (CheckCollisionPointRec(mp, btnQuit)) { 
                quit = true; 
            }
        }
    } else { // confirm screen
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, btnStart)) {
                // Always create a new game instance
                game = std::make_unique<Game>(selectedMode);
                startGame = true;
            } else if (CheckCollisionPointRec(mp, btnBack)) {
                selectedMode = NONE;
                confirmScreen = false;
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            selectedMode = NONE;
			startGame = false;
            confirmScreen = false;
        }
    }
}

static void DrawGradientButton(const Rectangle& r, const std::string& text, const Font& font, bool hovered, Color baseColor, Color hoverColor) {
	// Create gradient effect
	Color topColor = hovered ? hoverColor : baseColor;
	Color bottomColor = hovered ? 
		Color{(unsigned char)(hoverColor.r * 0.8f), (unsigned char)(hoverColor.g * 0.8f), (unsigned char)(hoverColor.b * 0.8f), 255} : 
		Color{(unsigned char)(baseColor.r * 0.8f), (unsigned char)(baseColor.g * 0.8f), (unsigned char)(baseColor.b * 0.8f), 255};
	
	// Draw gradient background
	DrawRectangleGradientV(r.x, r.y, r.width, r.height, topColor, bottomColor);
	
	// Draw border with subtle WINDOW_HEIGHTadow
	if (hovered) {
		DrawRectangleRoundedLines(r, 0.1f, 8, Color{255, 255, 255, 100});
	} else {
		DrawRectangleRoundedLines(r, 0.1f, 8, Color{0, 0, 0, 50});
	}
	
	// Draw text with WINDOW_HEIGHTadow
	Vector2 ts = MeasureTextEx(font, text.c_str(), 24, 2);
	Vector2 pos { r.x + (r.width - ts.x)/2.0f, r.y + (r.height - ts.y)/2.0f };
	
	// Text WINDOW_HEIGHTadow
	DrawTextEx(font, text.c_str(), {pos.x + 1, pos.y + 1}, 24, 2, Color{0, 0, 0, 100});
	// Main text
	DrawTextEx(font, text.c_str(), pos, 24, 2, WHITE);
}


void HomeScreen::Draw() const {
	// Animated background gradient
	float gradientWINDOW_HEIGHTift = sinf(animationTimer * 0.5f) * 20.0f;
	Color bg1 = Color{(unsigned char)(25 + gradientWINDOW_HEIGHTift), (unsigned char)(25 + gradientWINDOW_HEIGHTift), (unsigned char)(35 + gradientWINDOW_HEIGHTift), 255};
	Color bg2 = Color{(unsigned char)(45 + gradientWINDOW_HEIGHTift), (unsigned char)(45 + gradientWINDOW_HEIGHTift), (unsigned char)(65 + gradientWINDOW_HEIGHTift), 255};
	DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, bg1, bg2);
	
	// Title with glow effect
	std::string title = " RAYLIB CHESS ";
	Vector2 tsize = MeasureTextEx(titleFont, title.c_str(), 64, 4);
	float titleY = 80.0f + sinf(animationTimer) * 3.0f; 
	
	// Main title
	DrawTextEx(titleFont, title.c_str(), 
		{ (WINDOW_WIDTH - tsize.x)/2.0f, titleY }, 
		64, 4, Color{255, 235, 59, 255});

	if (!confirmScreen) {
		// Main menu buttons with different colors
		DrawGradientButton(btnEngineWhite, " Play vs Engine (White) ", font, hover(btnEngineWhite), 
			Color{100, 150, 200, 200}, Color{120, 170, 220, 255});
		DrawGradientButton(btnEngineBlack, " Play vs Engine (Black) ", font, hover(btnEngineBlack), 
			Color{80, 80, 80, 200}, Color{100, 100, 100, 255});
		DrawGradientButton(btnHuman, " Human vs Human ", font, hover(btnHuman), 
			Color{100, 180, 100, 200}, Color{120, 200, 120, 255});
		DrawGradientButton(btnQuit, " Quit ", font, hover(btnQuit), 
			Color{180, 100, 100, 200}, Color{200, 120, 120, 255});
		
		Vector2 hint = MeasureTextEx(font, "Choose your game mode", 20, 2);
		DrawTextEx(font, "Choose your game mode", 
			{ (WINDOW_WIDTH - hint.x)/2.0f, btnQuit.y + btnQuit.height + 30 }, 
			20, 2, Color{200, 200, 200, 180});
	} else {
		// Confirmation screen with better layout
		std::string modeText;
		switch (selectedMode) {
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
				modeText = ""; 
				break;
		}
		
		Vector2 ms = MeasureTextEx(font, modeText.c_str(), 28, 2);
		DrawTextEx(font, modeText.c_str(), 
			{ (WINDOW_WIDTH - ms.x)/2.0f, btnStart.y - 80 }, 
			28, 2, Color{255, 255, 255, 255});
		
		// Confirmation buttons
		DrawGradientButton(btnStart, " Start Game ", font, hover(btnStart), 
			Color{50, 150, 50, 200}, Color{70, 170, 70, 255});
		DrawGradientButton(btnBack, " Back ", font, hover(btnBack), 
			Color{150, 150, 50, 200}, Color{170, 170, 70, 255});
		
		Vector2 esc = MeasureTextEx(font, " Press ESC to go back ", 18, 2);
		DrawTextEx(font, " Press ESC to go back ", 
			{ (WINDOW_WIDTH - esc.x)/2.0f, btnBack.y + btnBack.height + 25 }, 
			18, 2, Color{160, 160, 160, 180});
	}
	
	// Subtle frame border
	DrawRectangleLines(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Color{100, 100, 100, 100});
}
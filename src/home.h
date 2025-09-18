#pragma once
#include <raylib.h>
#include <string>
#include <memory>

class Game;

class HomeScreen {
public:
	enum Mode {
		NONE = 0,
		VS_ENGINE_WHITE,
		VS_ENGINE_BLACK,
		HUMAN_VS_HUMAN
	};

	HomeScreen();
	~HomeScreen();

	bool quit = false;

    void Run();

	void Update();
	void Draw() const;
	bool ShouldStartGame() const { return startGame; }
	bool WantsQuit() const { return quit; }
	Mode GetMode() const { return selectedMode; }
	std::unique_ptr<Game>& GetGame() { return game; }
	bool UserIsWhite() const { return selectedMode == VS_ENGINE_WHITE; }
	bool PlayVsEngine() const { return selectedMode == VS_ENGINE_WHITE || selectedMode == VS_ENGINE_BLACK; }
	void Reset();

private:
	bool startGame = false;
	Mode selectedMode = NONE;
	Font font;
	Font titleFont;
	Font natoSans2; 
	float animationTimer;
	
	std::unique_ptr<Game> game;

	Rectangle btnEngineWhite{};
	Rectangle btnEngineBlack{};
	Rectangle btnHuman{};
	Rectangle btnQuit{};
	Rectangle btnBack{}; 
	Rectangle btnStart{};

	bool confirmScreen = false;

	void layoutButtons();
	bool hover(const Rectangle& r) const;
};
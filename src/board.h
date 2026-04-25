#ifndef BOARD_H
#define BOARD_H

#include <raylib.h>
#include <stack>
#include <vector>
#include "piece.h"
#include "stockfish.h"
#include "gameStateManager.h"
#include "home.h"
#include <string>
#include <unordered_map>

class Piece;
class HomeScreen;

class Board : public GameStateManager {
    HomeScreen::Mode mode;

    int originalRow, originalCol;
    bool isEngineRunning = false;
    bool userColor = true;

    std::string getStockfishMove();
    Texture2D whitePawn, whiteRook, whiteKnight, whiteBishop, whiteQueen, whiteKing;
    Texture2D blackPawn, blackRook, blackKnight, blackBishop, blackQueen, blackKing;
    Color green = {118,150,86,255};
    Color beige = {238,238,210,255};
    Sound moved;
    Sound capture;
    Sound gameend;
    Vector2 mousePos;
    std::unique_ptr<Stockfish> engine;

    void LoadTextures();
    void DrawPieces();
    void playMoveSound(int endX, int endY, bool isCapture);
    bool uciToCoords(const std::string& move, int &startX, int &startY, int &endX, int &endY);
    void makeEngineMove(const std::string& uciMove);
    void afterMoveHandle(int endX, int endY, int capturedPieceId);
    void startEngine();
    
public:
    Board(HomeScreen::Mode mode);
    ~Board();

    bool engineFailed = false;
    bool dragging = false;  
    Piece* selectedPiece;
    
    void InitializePieces();
    void Update();
    void Draw();
    void handleMove();
};

#endif // BOARD_H
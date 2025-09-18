#ifndef BOARD_H
#define BOARD_H

#include <raylib.h>
#include <stack>
#include <vector>
#include "piece.h"
#include "stockfish.h"
#include "simpleBoard.h"
#include "home.h"
#include <string>
#include <unordered_map>

class Piece;
class HomeScreen;

class Board : public simpleBoard {

private:

    HomeScreen::Mode mode;

    struct MoveHistory {
        int startX, startY, endX, endY;
        Piece movedPiece;
        Piece promotedPiece;
        Piece capturedPiece;
        
        // castling flag states before the move
        bool prevWhiteKingMoved = false;
        bool prevBlackKingMoved = false;
        bool prevWhiteKingsideRookMoved = false;
        bool prevWhiteQueensideRookMoved = false;
        bool prevBlackKingsideRookMoved = false;
        bool prevBlackQueensideRookMoved = false;
        
        // Flag to identify castling moves
        bool wasCastlingMove = false; 

        // En Passant flags
        bool wasEnPassant = false;
        int enPassantCapturedRow = -1;
        int enPassantCapturedCol = -1;
        
        // Pawn promotion flag
        bool wasPawnPromotion = false;

        // Half - Full Moves
        int halfMoves = 0;
        int fullMoves = 0;

        std::string prevFEN;
    };
    
    

    void LoadTextures();
    void DrawPieces();
    void checkCastelingAttempt(int endX, int endY);
    void checkPawnPromotion(int endX, int endY);
    void ExecuteMove(Piece& movedPiece, int mouseX, int mouseY);
    void simulateCasteling(Piece *selectedPiece, Piece &movedPiece, int endX, int endY);
    void playMoveSound(int endX, int endY, bool isCapture);
    bool uciToCoords(const std::string& move, int &startX, int &startY, int &endX, int &endY);
    void makeEngineMove(const std::string& uciMove);
    void afterMoveHandle(int endX, int endY, int capturedPieceId);
    void startEngine();
    
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
    bool isEngineRunning = false;
    
    int originalRow, originalCol;
    bool isPawnPromotion = false;

    bool userColor = true;

    std::string fen;

    std::stack<std::vector<Piece>> boardHistory;
    
public:
    Board(HomeScreen::Mode mode);
    ~Board();

    bool gameOver;
    bool victory = false;
    bool draw = false;
    bool isWhiteMov;
    bool isKingside = false;
    bool dragging = false;
    
    int enPassantCol = -1;
    int enPassantRow = -1;
    bool enPassant = false;
    bool enPassantCapture = false;

    int halfMovesCounter = 0;
    int fullMovesCounter = 0;
    
    Piece* selectedPiece;
    
    void InitializePieces();
    void Update();
    void Draw();
    void Undo();
    void saveState();
    void handleMove();
    void switchTurn();
    void SetPiece(int x, int y);
    void SetPiece(int x, int y, Piece& piece);

    std::vector<MoveHistory> moveHistory;
    std::unordered_map<std::string, int> positionHistory;
};

#endif // BOARD_H
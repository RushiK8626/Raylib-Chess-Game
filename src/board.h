#ifndef BOARD_H
#define BOARD_H

#include <raylib.h>
#include <stack>
#include <vector>
#include "piece.h"
#include "simpleBoard.h"
#include <string>

class Piece;

class Board : public simpleBoard {

private:

    struct MoveHistory {
        int startX, startY, endX, endY;
        Piece movedPiece;
        Piece promotedPiece;
        Piece capturedPiece;
        
        // castling flag states before the move
        bool prevWhiteKingMoved;
        bool prevBlackKingMoved;
        bool prevWhiteKingsideRookMoved;
        bool prevWhiteQueensideRookMoved;
        bool prevBlackKingsideRookMoved;
        bool prevBlackQueensideRookMoved;
        
        bool wasCastlingMove; // Flag to identify castling moves
        
        bool wasPawnPromotion;
    };

    void LoadTextures();
    void DrawPieces();
    void checkCastelingAttempt(int endX, int endY);
    void checkPawnPromotion(int endX, int endY);
    void ExecuteMove(Piece& movedPiece, int mouseX, int mouseY);
    void simulateCasteling(Piece *selectedPiece, Piece &movedPiece, int endX, int endY);
    void playMoveSound(int endX, int endY, bool isCurrentCheckmate, bool isCapture);
    Texture2D whitePawn, whiteRook, whiteKnight, whiteBishop, whiteQueen, whiteKing;
    Texture2D blackPawn, blackRook, blackKnight, blackBishop, blackQueen, blackKing;
    Color green = {118,150,86,255};
    Color beige = {238,238,210,255};
    Sound moved;
    Sound capture;
    Sound gameend;

    Vector2 mousePos;
    
    int originalRow, originalCol;
    bool isPawnPromotion;

public:
    Board(int cellSize, int offset);
    ~Board();

    int cellSize;
    int offset;
    bool gameOver;
    bool isWhiteMov;
    bool isKingside;
    bool dragging;
    Piece* selectedPiece;

    void InitializePieces();
    void Update();
    void Draw();
    void Undo();
    void handleMove();
    void switchTurn();
    bool IsCheckmate(bool whiteKing) const;
    void SetPiece(int x, int y);
    void SetPiece(int x, int y, Piece& piece);

    std::vector<MoveHistory> moveHistory;
};

#endif // BOARD_H
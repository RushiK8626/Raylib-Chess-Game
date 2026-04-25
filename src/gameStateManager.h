#ifndef BOARDCOPY_H
#define BOARDCOPY_H
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <raylib.h>
#include "piece.h"
class Piece;

class GameStateManager {
protected:
    bool isPawnPromotion = false;
    std::string fen;
    std::stack<std::vector<Piece>> boardHistory;

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

    bool HasKingMoved(bool isWhite) const;
    bool HasRookMoved(bool isWhite, bool isQueenside) const;
    void checkCastelingAttempt(int startX, int startY, int endX, int endY, const Piece* piece);
    void checkPawnPromotion(int endX, int endY, const Piece* piece);
    void ExecuteMove(Piece& movedPiece, int startX, int startY, int endX, int endY);
    void simulateCasteling(Piece &movedPiece, int startX, int startY, int endX, int endY);
    
public:
    GameStateManager();
    virtual ~GameStateManager() = default;

    bool isWhiteMov;
    bool gameOver;
    bool victory = false;
    bool draw = false;
    bool isKingside = false;
    bool casteling;
    bool whiteCastle;
    bool blackCastle;
    bool whiteQueenSideRookMoved;
    bool whiteKingSideRookMoved;
    bool blackQueenSideRookMoved;
    bool blackKingSideRookMoved;
    bool whiteKingMoved;
    bool blackKingMoved;
    int enPassantCol = -1;
    int enPassantRow = -1;
    bool enPassant = false;
    bool enPassantCapture = false;
    int halfMovesCounter = 0;
    int fullMovesCounter = 0;

    std::vector<MoveHistory> moveHistory;
    std::unordered_map<std::string, int> positionHistory;
    Piece board[8][8];
    const Piece& GetPiece(int row, int col) const;

    void Undo();
    void saveState();
    void switchTurn();
    void SetPiece(int x, int y);
    void SetPiece(int x, int y, Piece& piece);
    bool IsInCheck(bool whiteKing) const;
    bool FindKingPosition(bool whiteKing, int &kingRow, int &kingCol) const;
    bool IsSquareUnderAttack(int row, int col, bool attackedByWhite) const;
    bool ValidateAndExecuteMove(int fromX, int fromY, int toX, int toY, bool isWhiteTurn, Piece* selectedPiece);
};

#endif
#ifndef BOARDCOPY_H
#define BOARDCOPY_H

#include <raylib.h>
#include "piece.h"
class Piece;

class simpleBoard {
protected:
    bool whiteQueenSideRookMoved;
    bool whiteKingSideRookMoved;
    bool blackQueenSideRookMoved;
    bool blackKingSideRookMoved;
    bool whiteKingMoved;
    bool blackKingMoved;

    bool HasKingMoved(bool isWhite) const;
    bool HasRookMoved(bool isWhite, bool isQueenside) const;

public:
    simpleBoard();
    virtual ~simpleBoard() = default;
    
    bool casteling;
    bool whiteCastle;
    bool blackCastle;

    Piece board[8][8];
    bool IsInCheck(bool whiteKing) const;
    bool FindKingPosition(bool whiteKing, int &kingRow, int &kingCol) const;
    bool IsSquareUnderAttack(int row, int col, bool attackedByWhite) const;
    const Piece& GetPiece(int row, int col) const;
};

#endif
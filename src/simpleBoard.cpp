#include "simpleBoard.h"

simpleBoard::simpleBoard()  
{
    casteling = false;
    whiteCastle =false;
    blackCastle = false;
    whiteKingMoved = false;
    blackKingMoved = false;
    whiteQueenSideRookMoved = false;
    whiteKingSideRookMoved = false;
    blackKingSideRookMoved = false;
    blackQueenSideRookMoved = false;
}

bool simpleBoard::IsInCheck(bool whiteKing) const
{
    int kingRow, kingCol;
    if (!FindKingPosition(whiteKing, kingRow, kingCol))
        return false;
    return IsSquareUnderAttack(kingRow, kingCol, !whiteKing);
}

bool simpleBoard::FindKingPosition(bool whiteKing, int &kingRow, int &kingCol) const
{
    int targetId = whiteKing ? -5 : 5;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            if (board[row][col].id == targetId)
            {
                kingRow = row;
                kingCol = col;
                return true;
            }
        }
    }
    return false;
}

bool simpleBoard::IsSquareUnderAttack(int row, int col, bool attackedByWhite) const
{
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            Piece attacker = board[r][c];
            if (attacker.id == 0)
                continue;

            // Check if attacker is of opposite color
            bool isOpponentPiece = attackedByWhite ? (attacker.id < 0) : (attacker.id > 0);
            if (isOpponentPiece)
            {
                if (attacker.IsValidMove(c, r, col, row, *this))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

const Piece& simpleBoard::GetPiece(int row, int col) const 
{
    static const Piece emptyPiece;  // Static empty piece for invalid coordinates
    if (row >= 0 && row < 8 && col >= 0 && col < 8) 
    {
        return board[row][col];
    }
    return emptyPiece;
}

bool simpleBoard::HasRookMoved(bool isWhite, bool isKingside) const 
{
    if (isWhite) 
    {
        return isKingside ? whiteKingSideRookMoved : whiteQueenSideRookMoved;
    } else 
    {
        return isKingside ? blackKingSideRookMoved : blackQueenSideRookMoved;
    }
}

bool simpleBoard::HasKingMoved(bool isWhite) const 
{
    return isWhite ? whiteKingMoved : blackKingMoved;
}

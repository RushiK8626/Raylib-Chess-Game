#include "piece.h"
#include "simpleBoard.h"
#include "board.h"
#include "constants.h"
#include <cmath>

Piece::Piece() : id(0), texture({0}), col(0), row(0) {}

Piece::Piece(Texture2D texture, int col, int row, int id) :  id(id), texture(texture), col(col), row(row) {}

void Piece::Draw()
{
    Rectangle sourceRec = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle destRec = {
        static_cast<float>(col * CELL_SIZE + OFFSET + 10),
        static_cast<float>(row * CELL_SIZE + OFFSET + 10),
        static_cast<float>(CELL_SIZE - 20),
        static_cast<float>(CELL_SIZE - 20)
    };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
}

bool Piece::IsValidMove(int startX, int startY, int endX, int endY, const simpleBoard& board) const
{
    Piece dest = board.GetPiece(endY, endX);
    Piece start = board.GetPiece(startY, startX);
    if(endX < 0 || endX > 7 || endY < 0 || endY > 7)
    {
        return false;
    }
    if(dest.id != 0 && (dest.id * start.id > 0))
    {
        return false;
    }
    switch (std::abs(id)) {
        case 6:  // Pawn
            return IsValidPawnMove(startX, startY, endX, endY, start.id, dest.id, board);
        case 1:  // Rook
            return IsValidRookMove(startX, startY, endX, endY, start.id, dest.id, board);
        case 2:  // Knight
            return IsValidKnightMove(startX, startY, endX, endY, start.id, dest.id, board);
        case 3:  // Bishop
            return IsValidBishopMove(startX, startY, endX, endY, start.id, dest.id, board);
        case 4:  // Queen
            return IsValidQueenMove(startX, startY, endX, endY, start.id, dest.id, board);
        case 5:  // King
            return IsValidKingMove(startX, startY, endX, endY, start.id, dest.id, board);
        default:
            return false;
    }
}

bool Piece::IsValidPawnMove(int startX, int startY, int endX, int endY, int id, int destId, const simpleBoard& board) const
{
    int direction = (id > 0) ? 1 : -1; // White pawns move up, black pawns move down
    // Normal forward move
    if (startX == endX && destId == 0) {
        // Single square move
        if (endY == startY + direction) return true;
        
        // Initial double move
        if (((id > 0 && startY == 1) || (id < 0 && startY == 6)) && 
            endY == startY + 2 * direction && 
            board.GetPiece(startY + direction, startX).id == 0) {
            return true;
        }
    }
    
    // Capture diagonally
    if (std::abs(startX - endX) == 1 && 
        endY == startY + direction && 
        destId != 0 && 
        destId * id < 0) {
        return true;
    }

    // en Passant capture check
    Board* fullBoard = dynamic_cast<Board*>(const_cast<simpleBoard*>(&board));
    if (fullBoard && std::abs(startX - endX) == 1 && endY == startY + direction && destId == 0) {
        if (fullBoard->enPassantCol == endX && fullBoard->enPassantRow == endY) {
            fullBoard->enPassantCapture = true;
            return true;
        }
    }
    return false;
}

bool Piece::IsValidRookMove(int startX, int startY, int endX, int endY, int id, int destId, const simpleBoard& board) const
{
    if (startX != endX && startY != endY) return false;
    return IsPathClear(startX, startY, endX, endY, board);
}

bool Piece::IsValidKnightMove(int startX, int startY, int endX, int endY, int id, int destId, const simpleBoard& board) const
{
    int dx = std::abs(startX - endX);
    int dy = std::abs(startY - endY);
    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
}

bool Piece::IsValidBishopMove(int startX, int startY, int endX, int endY, int id, int destId, const simpleBoard& board) const
{
    if (std::abs(startX - endX) != std::abs(startY - endY)) return false;
    return IsPathClear(startX, startY, endX, endY, board);
}

bool Piece::IsValidQueenMove(int startX, int startY, int endX, int endY, int id, int destId, const simpleBoard& board) const
{
    return IsValidRookMove(startX, startY, endX, endY, id, destId, board) || 
        IsValidBishopMove(startX, startY, endX, endY, id, destId, board);
} 

bool Piece::IsValidKingMove(int startX, int startY, int endX, int endY, int id, int destId, const simpleBoard& board) const
{
    int dx = std::abs(startX - endX);
    int dy = std::abs(startY - endY);

    // Basic king move (one square in any direction)
    if (dx <= 1 && dy <= 1) return true;
    

    // Check for castling (2 squares horizontally)
    if (dy == 0 && dx == 2) 
    {
        // Check if intermediate position under attack;
        int intermediateCol = (startX + endX) / 2;
        if (board.IsSquareUnderAttack(startY, intermediateCol, id > 0)) return false;
        // Check if final position under attack
        if (board.IsSquareUnderAttack(startY, endX, id > 0)) return false;
        // Check if path is clear betweeen king and rook
        int rookId = (startY == 7) ? -1 : 1;
        int rookCol = endX < startX ? 0 : 7;
        if (endX < startX) 
        {
            // Queenside casteling

            if(board.GetPiece(startY, rookCol).id != rookId) return false;
            int x = startX - 1;
            while(x > 0) 
            {
                if(board.GetPiece(startY, x).id != 0) return false;
                x--;
            }
        }
        else if(endX > startX) 
        {
            // Kingside casteling

            if(board.GetPiece(startY, rookCol).id != rookId) return false;
            int x = startX + 1;
            while(x < 7) 
            {
                if(board.GetPiece(startY, x).id != 0) return false;
                x++;
            }
        }
        return true;
    }
    return false;
}

bool Piece::IsPathClear(int startX, int startY, int endX, int endY, const simpleBoard& board) const
{
    int dx = (endX > startX) ? 1 : (endX < startX) ? -1 : 0;
    int dy = (endY > startY) ? 1 : (endY < startY) ? -1 : 0;
    
    int x = startX + dx;
    int y = startY + dy;
    
    while (x != endX || y != endY) {
        if (board.GetPiece(y, x).id != 0) {
            return false; // Path is blocked
        }
        x += dx;
        y += dy;
    }
    
    return true;
}

void Piece::SetPosition(int newX, int newY) 
{ 
    col = newX; 
    row = newY; 
}

Texture2D Piece::getTexture()
{
    return texture;
}

int Piece::getRow()
{
    return row;
}

int Piece::getCol()
{
    return col;
}

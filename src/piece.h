#ifndef PIECE_H
#define PIECE_H

#include <raylib.h>

class GameStateManager;

class Piece {
public:
    Piece();
    Piece(Texture2D texture, int col, int row, int id);

    void Draw();
    void SetPosition(int newX, int newY);
    Texture2D getTexture();
    int getRow();
    int getCol();

    bool IsValidMove(int startX, int startY, int endX, int endY, const GameStateManager& board) const;

    int id;
    static const int cellSize = 80;
    static const int offset = 30;

private:
    Texture2D texture;
    int col, row;

    bool IsValidPawnMove(int startX, int startY, int endX, int endY, int id, int destId, const GameStateManager& board) const;
    bool IsValidRookMove(int startX, int startY, int endX, int endY, int id, int destId, const GameStateManager& board) const;
    bool IsValidKnightMove(int startX, int startY, int endX, int endY, int id, int destId, const GameStateManager& board) const;
    bool IsValidBishopMove(int startX, int startY, int endX, int endY, int id, int destId, const GameStateManager& board) const;
    bool IsValidQueenMove(int startX, int startY, int endX, int endY, int id, int destId, const GameStateManager& board) const;
    bool IsValidKingMove(int startX, int startY, int endX, int endY, int id, int destId, const GameStateManager& board) const;

    bool IsPathClear(int startX, int startY, int endX, int endY, const GameStateManager& board) const;
};

#endif // PIECE_H
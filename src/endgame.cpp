#include "endgame.h"

// Main stalemate check
bool IsStalemate(const Board &board, bool whiteToMove)
{
    if (board.IsInCheck(whiteToMove)) return false;

    // Try every possible move for the current player
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            const Piece& piece = board.board[fromRow][fromCol];
            if (piece.id == 0) continue;
            bool isWhitePiece = piece.id < 0;
            if (isWhitePiece != whiteToMove) continue;
            
            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    simpleBoard tempBoard = static_cast<const simpleBoard&>(board);
                    Piece& tempPiece = tempBoard.board[fromRow][fromCol];
                    if (tempPiece.IsValidMove(fromCol, fromRow, toCol, toRow, tempBoard)) {
                        // Simulate the move
                        tempBoard.board[toRow][toCol] = tempPiece;
                        tempBoard.board[fromRow][fromCol] = Piece();
                        tempBoard.board[toRow][toCol].SetPosition(toCol, toRow);

                        if (!tempBoard.IsInCheck(whiteToMove)) {
                            return false; // At least one legal move exists
                        }
                    }
                }
            }
        }
    }
    return true;
}


// Main checkmate check
bool IsCheckmate(const Board &board, bool whiteKing) 
{
    if (!board.IsInCheck(whiteKing))
        return false;

    int kingRow, kingCol;
    if (!board.FindKingPosition(whiteKing, kingRow, kingCol))
        return false;

    int kingId = whiteKing ? -5 : 5;
    
    // Try moving the king
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0)
                continue;

            int newRow = kingRow + dy;
            int newCol = kingCol + dx;

            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                const Piece& destPiece = board.GetPiece(newRow, newCol);
                if (destPiece.id * kingId > 0)
                    continue;

                // Create a non-const copy for move simulation
                simpleBoard tempBoard = static_cast<const simpleBoard&>(board);
                Piece& tempKing = tempBoard.board[kingRow][kingCol];
                
                // Move king on temporary board
                tempBoard.board[newRow][newCol] = tempKing;
                tempBoard.board[kingRow][kingCol] = Piece();
                tempBoard.board[newRow][newCol].SetPosition(newCol, newRow);

                if (!tempBoard.IsInCheck(whiteKing)) {
                    return false;
                }

            }
        }
    }

    // Try blocking or capturing with other pieces
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            const Piece& piece = board.GetPiece(fromRow, fromCol);
            if (piece.id == 0) continue;

            bool isPieceWhite = piece.id < 0;
            if (isPieceWhite != whiteKing) continue;

            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    // Create a non-const copy for move simulation
                    simpleBoard tempBoard = static_cast<const simpleBoard&>(board);
                    Piece& tempPiece = tempBoard.board[fromRow][fromCol];
                    
                    if (tempPiece.IsValidMove(fromCol, fromRow, toCol, toRow, tempBoard)) {
                        // Simulate the move
                        tempBoard.board[toRow][toCol] = tempPiece;
                        tempBoard.board[fromRow][fromCol] = Piece();
                        tempBoard.board[toRow][toCol].SetPosition(toCol, toRow);

                        if (!tempBoard.IsInCheck(whiteKing)) {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}
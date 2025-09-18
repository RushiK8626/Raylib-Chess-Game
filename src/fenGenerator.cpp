#include "fenGenerator.h"
#include "notationParser.h"

char fenGenerator::generateChar(int pieceId)
{
   switch(std::abs(pieceId))
    {
        case 1: return pieceId > 0 ? 'r' : 'R';
        case 2: return pieceId > 0 ? 'n' : 'N';
        case 3: return pieceId > 0 ? 'b' : 'B';
        case 4: return pieceId > 0 ? 'q' : 'Q';
        case 5: return pieceId > 0 ? 'k' : 'K';
        case 6: return pieceId > 0 ? 'p' : 'P';
        default: return '?';
    }
}

std::string fenGenerator::generateFEN(const Board &board)
{
    std::string fen = "";

    // Piece placement
    for (int row = 0; row < 8; ++row)
    {
        int emptyCount = 0;
        for(int col = 0; col < 8; ++col)
        {
            int pieceId = board.GetPiece(row, col).id;
            if(pieceId == 0)
            {
                emptyCount++;

            }
            else 
            {
                if (emptyCount > 0) {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                fen += generateChar(pieceId);
            }
        }
        if (emptyCount > 0) fen += std::to_string(emptyCount);
        if (row < 7) fen += '/';
    } 

    // Active color
    fen += board.isWhiteMov ? " w " : " b ";

    // Castling Rights
    fen += generateCastlingRightsString(board);

    // En Passant Targets
    fen += generateEnPassantTarget(board);

    // Half moves
    fen += std::to_string(board.halfMovesCounter) + ' ';

    // Full moves
    fen += std::to_string(board.fullMovesCounter);

    return fen;
}

std::string fenGenerator::generateCastlingRightsString(const Board& board)
{
    std::string castlingRights = "";
    if(!board.whiteKingMoved)
    {
        if(!board.whiteKingSideRookMoved) castlingRights += 'K';
        if(!board.whiteQueenSideRookMoved) castlingRights += 'Q';
    }
    if(!board.blackKingMoved)
    {
        if(!board.blackKingSideRookMoved) castlingRights += 'k';
        if(!board.blackQueenSideRookMoved) castlingRights += 'q';
    }

    if (castlingRights.length() > 0) castlingRights += ' ';
    else castlingRights += "- ";
    return castlingRights;
}

std::string fenGenerator::generateEnPassantTarget(const Board &board)
{
    int targetRow = board.enPassantRow;
    int targetCol = board.enPassantCol;

    if(targetCol == -1) return "- ";

    std::string enPassantCapture = getPositionNotation(targetRow, targetCol);
    enPassantCapture += ' ';
    return enPassantCapture;
}

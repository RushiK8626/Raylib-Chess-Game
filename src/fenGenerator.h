#ifndef FENGENERATOR_H
#define FENGENERATOR_H
#include "board.h"

class fenGenerator
{
private:
    static std::string generateCastlingRightsString(const Board& board);
    static std::string generateEnPassantTarget(const Board& board);
public:
    static char generateChar(int pieceId);
    static std::string generateFEN(const Board& board);
};

#endif
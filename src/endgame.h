#pragma once
#include "board.h"

bool IsStalemate(const Board& board, bool whiteToMove);
bool IsCheckmate(const Board &board, bool whiteKing);
#include "notationParser.h"
#include "fenGenerator.h"

std::string getPositionNotation(int row, int col)
{
    std::string algebraicNotation = "";

    // Columns
    switch(col)
    {
        case 0: algebraicNotation += 'a';
                break;
        case 1: algebraicNotation += 'b';
                break;
        case 2: algebraicNotation += 'c';
                break;
        case 3: algebraicNotation += 'd';
                break;
        case 4: algebraicNotation += 'e';
                break;
        case 5: algebraicNotation += 'f';
                break;
        case 6: algebraicNotation += 'g';
                break;
        case 7: algebraicNotation += 'h';
                break;
    }

    // Rows (rank 8 at row 0 down to rank 1 at row 7)
    switch(row)
    {
        case 0: algebraicNotation += '8';
                break;
        case 1: algebraicNotation += '7';
                break;
        case 2: algebraicNotation += '6';
                break;
        case 3: algebraicNotation += '5';
                break;
        case 4: algebraicNotation += '4';
                break;
        case 5: algebraicNotation += '3';
                break;
        case 6: algebraicNotation += '2';
                break;
        case 7: algebraicNotation += '1';
                break;
    }

    return algebraicNotation;
}
#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr int CELL_SIZE = 80;
constexpr int OFFSET = 30;
constexpr int CELL_COUNT = 8;
constexpr int SMALLER_CELL_SIZE = 60;
constexpr int SMALLER_OFFSET = 20;
constexpr int WINDOW_WIDTH = CELL_SIZE * 8 + 2 * OFFSET;
constexpr int WINDOW_HEIGHT = CELL_SIZE * 8 + 2 * OFFSET;
constexpr int SMALLER_WINDOW_WIDTH = SMALLER_CELL_SIZE * 8 + 2 * SMALLER_OFFSET;
constexpr int SMALLER_WINDOW_HEIGHT = SMALLER_CELL_SIZE * 8 + 2 * SMALLER_OFFSET;

#endif // CONSTANTS_H
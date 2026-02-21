# Raylib Chess Game

A simple chess game built with [Raylib](https://www.raylib.com/) in C++, featuring a clean UI, sound effects, and an AI opponent powered by [Stockfish](https://stockfishchess.org/).

## Features

- **Human vs Human** – Two players on the same machine
- **Human vs Engine** – Play as White or Black against Stockfish AI
- Move validation and chess rules enforcement
- FEN generation and algebraic notation parsing
- Sound effects and custom fonts

## Requirements

- [Raylib 5.0](https://github.com/raysan5/raylib/releases/tag/5.0)
- [Stockfish](https://stockfishchess.org/download/) (for AI mode) – place the binary in the project root
- C++14 compiler (g++ / clang++)

## Building

### Linux / macOS
```bash
make
```

### Windows (MinGW)
```bash
mingw32-make
```

The output binary will be named `game` (or `game.exe` on Windows).

## Running

```bash
./game
```

Select a game mode from the home screen:
- **vs Engine (White)** – You play as White against Stockfish
- **vs Engine (Black)** – You play as Black against Stockfish
- **Human vs Human** – Local two-player match

## Project Structure

```
src/          C++ source files
graphics/     Piece and board images
sounds/       Sound effect files
font/         Custom fonts
lib/          Raylib static library
```

## License

This project uses Raylib which is licensed under the [zlib license](https://www.raylib.com/license.html).

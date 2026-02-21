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
- [Stockfish](https://stockfishchess.org/download/) (for AI mode) – create folder named stockfish in root directory and place stockfish.exe in it
- C++14 compiler (g++ / clang++)
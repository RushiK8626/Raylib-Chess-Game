#include <raylib.h>
#include <cstdlib>
#include <stack>
#include <string>
#include <filesystem>
#include "board.h"
#include "piece.h"
#include "endgame.h"
#include "fenGenerator.h"
#include "constants.h"

Board::Board(HomeScreen::Mode mode)
    : GameStateManager(),
      mode(mode)
{
    moved = LoadSound("sounds/move-self.wav");
    capture = LoadSound("sounds/capture.wav");
    gameend = LoadSound("sounds/game-end.wav");
    isWhiteMov = true;
    selectedPiece = nullptr;
    dragging = false;
    gameOver = false;

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            board[row][col] = Piece();
        }
    }
    LoadTextures();
    InitializePieces();
    if(mode == HomeScreen::Mode::VS_ENGINE_BLACK || mode == HomeScreen::Mode::VS_ENGINE_WHITE) 
    {
        try 
        {
            // Try multiple locations for stockfish
            char exePath[MAX_PATH];
            GetModuleFileNameA(NULL, exePath, MAX_PATH);
            std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
            std::filesystem::path cwd = std::filesystem::current_path();
            
            std::vector<std::filesystem::path> candidates = {
                exeDir / "stockfish" / "stockfish.exe",
                cwd / "stockfish" / "stockfish.exe",
            };
            
            std::string sfPath;
            for (const auto& candidate : candidates) 
            {
                if (std::filesystem::exists(candidate)) 
                {
                    sfPath = candidate.string();
                    break;
                }
            }
            
            if (sfPath.empty()) 
            {
                std::cerr << "Stockfish not found. Checked:" << std::endl;
                for (const auto& candidate : candidates) 
                {
                    std::cerr << "  - " << candidate.string() << std::endl;
                }
                engine.reset();
                engineFailed = true;
                return;
            }
            
            engine = std::make_unique<Stockfish>(sfPath);

            userColor = (mode == HomeScreen::Mode::VS_ENGINE_WHITE) ? true : false;
            startEngine();
            if (!isEngineRunning) 
            {
                engine.reset();
                engineFailed = true;
                return;
            }
        }
        catch (const std::exception& e) 
        {
            std::cerr << "Failed to load Stockfish: " << e.what() << std::endl;
            engine.reset();
            engineFailed = true;
            return;
        }
    }
    fen = fenGenerator::generateFEN(*this);
    positionHistory[fen]++;
}

Board::~Board()
{
    // Unload textures
    UnloadTexture(whitePawn);
    UnloadTexture(whiteRook);
    UnloadTexture(whiteKnight);
    UnloadTexture(whiteBishop);
    UnloadTexture(whiteQueen);
    UnloadTexture(whiteKing);

    UnloadTexture(blackPawn);
    UnloadTexture(blackRook);
    UnloadTexture(blackKnight);
    UnloadTexture(blackBishop);
    UnloadTexture(blackQueen);
    UnloadTexture(blackKing);

    // Unload sounds
    if(moved.frameCount > 0) UnloadSound(moved);
    if(capture.frameCount > 0) UnloadSound(capture);
}

void Board::LoadTextures()
{
    // Load white piece textures
    whitePawn = LoadTexture("graphics/pawnWhite.png");
    whiteRook = LoadTexture("graphics/rookWhite.png");
    whiteKnight = LoadTexture("graphics/knightWhite.png");
    whiteBishop = LoadTexture("graphics/bishopWhite.png");
    whiteQueen = LoadTexture("graphics/queenWhite.png");
    whiteKing = LoadTexture("graphics/kingWhite.png");

    // Load black piece textures
    blackPawn = LoadTexture("graphics/pawnBlack.png");
    blackRook = LoadTexture("graphics/rookBlack.png");
    blackKnight = LoadTexture("graphics/knightBlack.png");
    blackBishop = LoadTexture("graphics/bishopBlack.png");
    blackQueen = LoadTexture("graphics/queenBlack.png");
    blackKing = LoadTexture("graphics/kingBlack.png");
}

void Board::InitializePieces()
{
    // White pieces
    board[7][0] = Piece(whiteRook, 0, 7, -1);
    board[7][1] = Piece(whiteKnight, 1, 7, -2);
    board[7][2] = Piece(whiteBishop, 2, 7, -3);
    board[7][3] = Piece(whiteQueen, 3, 7, -4);
    board[7][4] = Piece(whiteKing, 4, 7, -5);
    board[7][5] = Piece(whiteBishop, 5, 7, -3);
    board[7][6] = Piece(whiteKnight, 6, 7, -2);
    board[7][7] = Piece(whiteRook, 7, 7, -1);

    // White pawns
    for (int col = 0; col < 8; ++col)
    {
        board[6][col] = Piece(whitePawn, col, 6, -6);
    }

    // Black pieces
    board[0][0] = Piece(blackRook, 0, 0, 1);
    board[0][1] = Piece(blackKnight, 1, 0, 2);
    board[0][2] = Piece(blackBishop, 2, 0, 3);
    board[0][3] = Piece(blackQueen, 3, 0, 4);
    board[0][4] = Piece(blackKing, 4, 0, 5);
    board[0][5] = Piece(blackBishop, 5, 0, 3);
    board[0][6] = Piece(blackKnight, 6, 0, 2);
    board[0][7] = Piece(blackRook, 7, 0, 1);

    // Black pawns
    for (int col = 0; col < 8; ++col)
    {
        board[1][col] = Piece(blackPawn, col, 1, 6);
    }
}

void Board::Update()
{
    handleMove();
}

void Board::Draw()
{
    for (int row = 0; row < 8; ++row)
    {
        for (int column = 0; column < 8; ++column)
        {
            Color cellColor = (row + column) % 2 == 0 ? green : beige;
            DrawRectangle(
                OFFSET + column * CELL_SIZE,
                OFFSET + row * CELL_SIZE,
                CELL_SIZE, CELL_SIZE, cellColor);
        }
    }
    DrawPieces();
}

void Board::DrawPieces()
{
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            if (board[row][col].id != 0 && &board[row][col] != selectedPiece)
            {
                board[row][col].Draw();
            }
        }
    }

    // If piece is selected for move adjust its position according mouse position
    if (dragging && selectedPiece != nullptr)
    {
        Rectangle sourceRec = { 0, 0, (float)selectedPiece->getTexture().width, (float)selectedPiece->getTexture().height };
        Rectangle destRec = {
            static_cast<float>(mousePos.x - CELL_SIZE / 2), 
            static_cast<float>(mousePos.y - CELL_SIZE / 2), 
            static_cast<float>(CELL_SIZE - 20), 
            static_cast<float>(CELL_SIZE - 20) 
        };
        Vector2 origin = { 0, 0 };

        DrawTexturePro(selectedPiece->getTexture(), sourceRec, destRec, origin, 0.0f, WHITE);
    }
}

void Board::handleMove()
{
    // ENGINE MODE: Let the engine make moves
    if((mode == HomeScreen::Mode::VS_ENGINE_WHITE && !isWhiteMov)|| (mode == HomeScreen::Mode::VS_ENGINE_BLACK && isWhiteMov))
    {
        if (!engine || engineFailed) return;
        std::string uciMove = getStockfishMove();
        makeEngineMove(uciMove);
        return;
    }
    
    // HUMAN MODE: Handle mouse input
    mousePos = GetMousePosition();
    int mouseX = (mousePos.x - OFFSET) / CELL_SIZE;
    int mouseY = (mousePos.y - OFFSET) / CELL_SIZE;

    // Check if mouse is within board bounds
    if (mouseX >= 0 && mouseX < 8 && mouseY >= 0 && mouseY < 8)
    {
        // MOUSE PRESS: Select piece
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Piece &clickedPiece = board[mouseY][mouseX];

            if (clickedPiece.id != 0) // Check if a piece exists
            {
                selectedPiece = &clickedPiece;
                originalRow = mouseY;
                originalCol = mouseX;
                dragging = true;
            }
        }

        // MOUSE RELEASE: Attempt move (UI + Chess Logic)
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && selectedPiece != nullptr)
        {
            if (mouseX >= 0 && mouseX < 8 && mouseY >= 0 && mouseY < 8)
            {
                // CHESS LOGIC: Validate and simulate move
                bool isMoveValid = ValidateAndExecuteMove(originalCol, originalRow, mouseX, mouseY, isWhiteMov, selectedPiece);

                if (isMoveValid)
                {
                    // Move is valid, execute it
                    Piece movedPiece = *selectedPiece;
                    int capturedPieceId = board[mouseY][mouseX].id;

                    // Call GameStateManager::ExecuteMove with correct parameters
                    GameStateManager::ExecuteMove(movedPiece, originalCol, originalRow, mouseX, mouseY);
                    
                    // Handle UI consequences (sounds, etc)
                    afterMoveHandle(mouseX, mouseY, capturedPieceId);
                }
                else
                {
                    // Move is invalid, reset piece position and UI state
                    selectedPiece->SetPosition(originalCol, originalRow);
                    dragging = false;
                    casteling = false;
                    isPawnPromotion = false;
                    selectedPiece = nullptr;
                }
            }
            else
            {
                // Move outside board - reset
                selectedPiece->SetPosition(originalCol, originalRow);
                dragging = false;
                selectedPiece = nullptr;
            }
        }
    }
    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        // Mouse released outside board - deselect
        dragging = false;
        selectedPiece = nullptr;
    }
}

void Board::afterMoveHandle(int endX, int endY, int capturedPieceId)
{
    // Save current board state
    switchTurn();

    fen = fenGenerator::generateFEN(*this);
    positionHistory[fen]++;

    bool isCurrentCheckmate = IsCheckmate(static_cast<const Board&>(*this), isWhiteMov);
    bool isCurrentStalemate = IsStalemate(static_cast<const Board&>(*this), isWhiteMov);
    // Handle checkmate
    if (isCurrentCheckmate)
    {
        PlaySound(gameend);
        gameOver = true; // Set gameover flag true
        victory = true;
        return;
    }
    else if (isCurrentStalemate) 
    {
        PlaySound(gameend);
        gameOver = true;
        draw = true;
        return;
    }
    
    playMoveSound(endX, endY, capturedPieceId != 0);

    dragging = false;
    selectedPiece = nullptr;
    enPassant = false;
    enPassantCapture = false;
}

void Board::playMoveSound(int endX, int endY, bool isCapture)
{
    if (isCapture) PlaySound(capture);
    else PlaySound(moved);
}

void Board::startEngine()
{
    // Initialize UCI once at startup (engine process persists)
    engine->sendCommand("uci\n");
    {
        const std::string r = engine->readResponse();
        isEngineRunning = (r.find("uciok") != std::string::npos);
    }
    // Start with a clean engine state; don't start a search until it's the engine's turn
    engine->sendCommand("ucinewgame\n");
    engine->sendCommand("isready\n");
    engine->readResponse(); // expect readyok
}

std::string Board::getStockfishMove()
{
    try
    {
        // Ensure previous searches are stopped and engine is ready
        engine->sendCommand("stop\n");
        engine->sendCommand("isready\n");
        engine->readResponse();

        // Set exact position from FEN (includes side to move) and ask for best move
        engine->sendCommand("position fen " + fen + "\n");
        engine->sendCommand("go depth 20\n");

        std::string resp = engine->readResponse();
        // Extract just the move after "bestmove " if present
        std::string bestMove = resp;
        const std::string prefix = "bestmove ";
        auto pos = resp.find(prefix);
        if (pos != std::string::npos) {
            auto end = resp.find_first_of(" \n\r\t", pos + prefix.size());
            if (end != std::string::npos) bestMove = resp.substr(pos + prefix.size(), end - (pos + prefix.size()));
            else bestMove = resp.substr(pos + prefix.size());
        }
        return bestMove;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return "";
    }
}

bool Board::uciToCoords(const std::string& move, int &startX, int &startY, int &endX, int &endY) {
    if (move.size() < 4) return false;

    startX = move[0] - 'a';           // 'a'->0, 'b'->1
    startY = 8 - (move[1] - '0');    // '1'->7, '2'->6
    endX = move[2] - 'a';
    endY = 8 - (move[3] - '0');

    return true;
}

void Board::makeEngineMove(const std::string& uciMove) {
    int startX, startY, endX, endY;
    if(!uciToCoords(uciMove, startX, startY, endX, endY)) return;

    // Work on a copy to avoid aliasing the board cell that we clear inside ExecuteMove
    Piece movedPiece = board[startY][startX];
    if (movedPiece.id == 0) return; // No piece abort

    // Safety: ensure engine moves the correct color for the current turn
    if ((isWhiteMov && movedPiece.id >= 0) || (!isWhiteMov && movedPiece.id <= 0)) {
        // The UCI move doesn't match side to move; ignore
        return;
    }
    selectedPiece = &movedPiece;

    // Set source for ExecuteMove bookkeeping
    originalCol = startX;
    originalRow = startY;

    // Call GameStateManager functions with new signatures
    checkCastelingAttempt(startX, startY, endX, endY, &movedPiece);
    checkPawnPromotion(endX, endY, &movedPiece);

    int capturedPieceId = board[endY][endX].id;
    // Call GameStateManager::ExecuteMove with correct parameters
    GameStateManager::ExecuteMove(movedPiece, startX, startY, endX, endY);

    afterMoveHandle(endX, endY, capturedPieceId);
}

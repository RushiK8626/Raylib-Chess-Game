#include <raylib.h>
#include <cstdlib>
#include <stack>
#include <string>
#include "board.h"
#include "piece.h"
#include "endgame.h"
#include "fenGenerator.h"
#include "constants.h"

Board::Board(HomeScreen::Mode mode)
    : simpleBoard(),
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
        engine = std::make_unique<Stockfish>("../stockfish/stockfish.exe"); //Use absolute or relative path to stockfish executable here
        userColor = (mode == HomeScreen::Mode::VS_ENGINE_WHITE) ? true : false;
        startEngine();
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

void Board::Undo()
{
    if (moveHistory.empty()) return;

    // Remove last move from history vector
    if (moveHistory.back().movedPiece.id > 0) moveHistory.pop_back();
    MoveHistory lastMove = moveHistory.back();
    moveHistory.pop_back();

    // Remove from past board history
    if (!boardHistory.empty()) boardHistory.pop();

    positionHistory[fenGenerator::generateFEN(*this)]--;

    // Update the position to previous one on the board
    SetPiece(lastMove.startY, lastMove.startX, lastMove.movedPiece);

    if(lastMove.wasEnPassant) 
    {
        SetPiece(lastMove.endY, lastMove.endX); // Remove the pawn from the destination square
        SetPiece(lastMove.enPassantCapturedRow, lastMove.enPassantCapturedCol, lastMove.capturedPiece);
    }
    else if (lastMove.capturedPiece.id != 0) 
    {
        SetPiece(lastMove.endY, lastMove.endX, lastMove.capturedPiece);
    } else 
    {
        SetPiece(lastMove.endY, lastMove.endX);
    }

    halfMovesCounter = lastMove.halfMoves;
    fullMovesCounter = lastMove.fullMoves;

    // Update castling flags after undoing the move
    whiteKingMoved = lastMove.prevWhiteKingMoved;
    blackKingMoved = lastMove.prevBlackKingMoved;
    whiteKingSideRookMoved = lastMove.prevWhiteKingsideRookMoved;
    whiteQueenSideRookMoved = lastMove.prevWhiteQueensideRookMoved;
    blackKingSideRookMoved = lastMove.prevBlackKingsideRookMoved;
    blackQueenSideRookMoved = lastMove.prevBlackQueensideRookMoved;

    fen = lastMove.prevFEN;

    // Handle if move was castling move
    if (lastMove.wasCastlingMove) 
    {
        if (lastMove.endX < lastMove.startX) {
            // Queenside castling - move rook back from column 3 to column 0
            Piece rook = GetPiece(lastMove.startY, 3);
            SetPiece(lastMove.startY, 0, rook);
            SetPiece(lastMove.startY, 3); 
        } else {
            // Kingside castling - move rook back from column 5 to column 7
            Piece rook = GetPiece(lastMove.startY, 5);
            SetPiece(lastMove.startY, 7, rook);
            SetPiece(lastMove.startY, 5);
        }

        if (lastMove.movedPiece.id < 0) {  
            whiteCastle = false;
        } else {  
            blackCastle = false;
        }
    }

    else if(lastMove.wasPawnPromotion)
    {
        // Undo pawn promotion
        SetPiece(lastMove.startY, lastMove.startX, lastMove.movedPiece);
        SetPiece(lastMove.endY, lastMove.endX);
    }

    // Play sound and switch turn
    PlaySound(capture);
    switchTurn();
}

void Board::handleMove()
{
    if((mode == HomeScreen::Mode::VS_ENGINE_WHITE && !isWhiteMov)|| (mode == HomeScreen::Mode::VS_ENGINE_BLACK && isWhiteMov))
    {
        std::string uciMove = getStockfishMove();
        makeEngineMove(uciMove);
    }
    else
    {
        mousePos = GetMousePosition();
        int mouseX = (mousePos.x - OFFSET) / CELL_SIZE;
        int mouseY = (mousePos.y - OFFSET) / CELL_SIZE;

        // Check if mouse is within board bounds
        if (mouseX >= 0 && mouseX < 8 && mouseY >= 0 && mouseY < 8)
        {
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

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && selectedPiece != nullptr)
            {
                if (mouseX >= 0 && mouseX < 8 && mouseY >= 0 && mouseY < 8)
                {
                    // Prevent moving opponent's pieces
                    if ((selectedPiece->id < 0 && !isWhiteMov) || (selectedPiece->id > 0 && isWhiteMov))
                    {
                        selectedPiece->SetPosition(originalCol, originalRow);
                        dragging = false;
                        selectedPiece = nullptr;
                        return;
                    }
                    // Check for castling
                    checkCastelingAttempt(mouseX, mouseY);

                    // Check for pawn promption
                    checkPawnPromotion(mouseX, mouseY);
                    
                    // Validate move based on piece rules
                    if (!selectedPiece->IsValidMove(originalCol, originalRow, mouseX, mouseY, *this))
                    {
                        selectedPiece->SetPosition(originalCol, originalRow);
                        dragging = false;
                        casteling = false;
                        isPawnPromotion = false;
                        selectedPiece = nullptr;
                        return;
                    }
                    
                    // Simulate move to check for self-check
                    Piece tempBoard[8][8];
                    memcpy(tempBoard, board, sizeof(board));

                    // Simulate moves 
                    Piece movedPiece = *selectedPiece;
                    // Piece capturedPiece = GetPiece(mouseY, mouseX);

                    // Simulate castling if selected
                    if(casteling) simulateCasteling(selectedPiece, movedPiece, mouseX, mouseY);
                    else 
                    {
                        // Normal moves
                        movedPiece.SetPosition(mouseX, mouseY);
                        board[originalRow][originalCol] = Piece();
                        board[mouseY][mouseX] = movedPiece;
                    }

                    // Check if move puts own king in check
                    bool kingInCheck = IsInCheck(isWhiteMov);

                    if (kingInCheck)
                    {
                        selectedPiece->SetPosition(originalCol, originalRow);
                        dragging = false;
                        casteling = false;
                        selectedPiece = nullptr;
                        memcpy(board, tempBoard, sizeof(board));
                        return;
                    }
        
                    // Restore board to previous state
                    memcpy(board, tempBoard, sizeof(board));

                    // Perform the actual move
                    movedPiece = *selectedPiece;
                    int capturedPieceId = board[mouseY][mouseX].id;

                    ExecuteMove(movedPiece, mouseX, mouseY);

                    afterMoveHandle(mouseX, mouseY, capturedPieceId);
                }
                else
                {
                    // Prevent moving opponent's pieces
                    if ((selectedPiece->id < 0 && !isWhiteMov) || (selectedPiece->id > 0 && isWhiteMov))
                    {
                        selectedPiece->SetPosition(originalCol, originalRow);
                        dragging = false;
                        selectedPiece = nullptr;
                        return;
                    }
                }
            }
        }
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            dragging = false;
            selectedPiece = nullptr;
        }
    }
}

void Board::switchTurn()
{
    isWhiteMov = !isWhiteMov;
}

// Execute the move
void Board::ExecuteMove(Piece& movedPiece, int endX, int endY) 
{
    Piece capturedPiece = GetPiece(endY, endX);

    MoveHistory move;
    move.startX = originalCol;
    move.startY = originalRow;
    move.endX = endX;
    move.endY = endY;
    move.movedPiece = movedPiece;
    move.capturedPiece = capturedPiece;
    move.wasCastlingMove = false;
    
    // Save current flag states before move
    move.prevWhiteKingMoved = whiteKingMoved;
    move.prevBlackKingMoved = blackKingMoved;
    move.prevWhiteKingsideRookMoved = whiteKingSideRookMoved;
    move.prevWhiteQueensideRookMoved = whiteQueenSideRookMoved;
    move.prevBlackKingsideRookMoved = blackKingSideRookMoved;
    move.prevBlackQueensideRookMoved = blackQueenSideRookMoved;

    move.wasPawnPromotion = isPawnPromotion;

    move.halfMoves = halfMovesCounter;
    move.fullMoves = fullMovesCounter;

    move.prevFEN = fen;

    halfMovesCounter++;
    fullMovesCounter++;

    // Casteling move
    if(casteling) 
    {
        movedPiece.SetPosition(endX, endY);
        int rookFromCol, rookToCol;
        if(isKingside) {rookFromCol = 7, rookToCol = endX - 1;}
        else {rookFromCol = 0; rookToCol = endX + 1;}
        
        Piece rook = board[endY][rookFromCol];
        rook.SetPosition(rookToCol, endY);
        board[endY][rookFromCol] = Piece();
        board[endY][rookToCol] = rook;
        board[originalRow][originalCol] = Piece();
        board[endY][endX] = movedPiece;
        
        // Update casteling flags
        if(isWhiteMov) 
        {
            whiteCastle = true;
            whiteKingMoved = true;
            if(isKingside) whiteKingSideRookMoved = true;
            else whiteQueenSideRookMoved = true;
        }
        else 
        {
            blackCastle = true;
            blackKingMoved = true;
            if(isKingside) whiteKingSideRookMoved = true;
            else whiteQueenSideRookMoved = true;
        }
        move.wasCastlingMove = true;
        casteling = false;
        halfMovesCounter++;
    }

    else if(isPawnPromotion) 
    {
        Piece promoted;
        Piece capturedPiece = GetPiece(endY, endX);
        if (movedPiece.id < 0) promoted = Piece(whiteQueen, endX, endY, -4);
        else promoted = Piece(blackQueen, endX, endY, 4);

        board[originalRow][originalCol] = Piece();
        board[endY][endX] = promoted;
        move.promotedPiece = promoted;
        move.capturedPiece = capturedPiece;
        isPawnPromotion = false;
        halfMovesCounter = 0;
        PlaySound(capture);
    }

    else if(enPassantCapture)
    {
        if (abs(movedPiece.id) == 6 && enPassantCol == endX && enPassantRow == endY && board[endY][endX].id == 0) {
            int capturedPawnRow = (movedPiece.id < 0) ? endY + 1 : endY - 1;
            move.capturedPiece = GetPiece(capturedPawnRow, endX);
            board[capturedPawnRow][endX] = Piece(); // Remove the captured pawn
            // Move the pawn to the destination
            movedPiece.SetPosition(endX, endY);
            board[originalRow][originalCol] = Piece();
            board[endY][endX] = movedPiece;
            enPassant = false;
            enPassantCapture = false;
            move.wasEnPassant = true;
            move.enPassantCapturedRow = (movedPiece.id < 0) ? endY + 1 : endY - 1;
            move.enPassantCapturedCol = endX;
            halfMovesCounter = 0;
            PlaySound(capture);
        }
    }

    else 
    {   
        // Normal moves
        movedPiece.SetPosition(endX, endY);
        board[originalRow][originalCol] = Piece();
        board[endY][endX] = movedPiece;
        enPassant = false;
        enPassantCapture = false;

        // check for en passant if this is a pawn move
        if (abs(movedPiece.id) == 6) 
        {
            // Check for pawn double move
            if (abs(endY - originalRow) == 2) 
            {
                enPassantCol = endX;
                enPassantRow = (originalRow + endY) / 2;
                enPassant = true;
                enPassantCapture = false;
            } else 
            {
                enPassant = false;
                enPassantCol = -1;
                enPassantRow = -1;
                enPassantCapture = false;
            }
            halfMovesCounter = 0;
        } 
        else 
        {
            enPassant = false;
            enPassantCapture = false;
            enPassantCol = -1;
            enPassantRow = -1;
        }

        if(capturedPiece.id != 0) halfMovesCounter = 0;


        // Update some flags
        int id = movedPiece.id;
        if(id == -5) whiteKingMoved = true;
        else if (id == 5) blackKingMoved = true;
        
        if(!whiteQueenSideRookMoved && id == -1 && originalRow == 7 && originalCol == 0) whiteQueenSideRookMoved = true;
        else if(!whiteKingSideRookMoved && id == -1 && originalRow == 7 && originalCol == 7) whiteKingSideRookMoved = true;
        else if(!blackQueenSideRookMoved && id == 1 && originalRow == 0 && originalCol == 0) blackQueenSideRookMoved = true;
        else if(!blackKingSideRookMoved && id == 1 && originalRow == 0 && originalCol == 7) blackKingSideRookMoved = true;
    }
    moveHistory.push_back(move);
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

// Check if castling attempt is valid
void Board::checkCastelingAttempt(int endX, int endY)
{
    casteling = false;
    // Check casteling attempted
    if (!IsInCheck(isWhiteMov) && (std::abs(selectedPiece->id) == 5 && ((isWhiteMov && !whiteCastle) || (!isWhiteMov && !blackCastle)))) // King
    {
        if(endY == originalRow && ((isWhiteMov && selectedPiece->getRow() == 7) || (!isWhiteMov && selectedPiece->getRow() == 0))) {
            if (originalCol - endX == 2) {isKingside = false, casteling = true;}
            else if (endX - originalCol == 2) {isKingside = true, casteling = true;}
        }
    }

    // Check if king or participating rook moved earlier
    if(HasKingMoved(isWhiteMov)) {casteling = false; return;}
    if(HasRookMoved(isWhiteMov, isKingside)) {casteling = false; return;}
}

void Board::simulateCasteling(Piece *selectedPiece, Piece &movedPiece, int endX, int endY) 
{
    if((isWhiteMov && whiteCastle) || (!isWhiteMov && blackCastle)) 
    {
        dragging = false;
        casteling = false;
        selectedPiece = nullptr;
        return; // Player already did casteling
    }

    // Move king for simulation
    movedPiece.SetPosition(endX, endY);
    board[originalRow][originalCol] = Piece();
    board[endY][endX] = movedPiece;
    
    int rookFromCol = (isKingside) ? 7 : 0;
    int rookToCol = (isKingside) ? (endX - 1) : (endX + 1);

    // Simulate the rook movement
    Piece rook = board[endY][rookFromCol];
    board[endY][rookFromCol] = Piece();
    board[endY][rookToCol] = rook;
}

void Board::checkPawnPromotion(int endX, int endY) 
{
    int id = selectedPiece->id;
    if(id != 6 && id != -6) return;

    int targetRow = isWhiteMov ? 0 : 7;
    if(endY != targetRow) return;

    isPawnPromotion = true;
}

// Play move sounds
void Board::playMoveSound(int endX, int endY, bool isCapture)
{
    if (isCapture) PlaySound(capture);
    else PlaySound(moved);
}

void Board::saveState()
{
    std::vector<Piece> snapshot;
    snapshot.reserve(64);
    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
            snapshot.push_back(board[row][col]);
    boardHistory.push(snapshot);
}

// Setters
void Board::SetPiece(int row, int col, Piece& piece)
{
    piece.SetPosition(col, row);
    board[row][col] = piece;
}

void Board::SetPiece(int row, int col)
{
    board[row][col] = Piece();
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
        // Choose one: movetime or depth; movetime is safer with GUI frame loop
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

// Convert UCI like "e2e4" → startX, startY, endX, endY
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

    checkCastelingAttempt(endX, endY);
    checkPawnPromotion(endX, endY);

    int capturedPieceId = board[endY][endX].id;
    ExecuteMove(movedPiece, endX, endY);

    afterMoveHandle(endX, endY, capturedPieceId);
}

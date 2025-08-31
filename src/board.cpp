#include "board.h"
#include "piece.h"
#include <raylib.h>
#include <iostream>
#include<cstdlib>
#include <stdio.h>
#include <stack>
#include <string>

Board::Board(int cellSize, int offset) : simpleBoard(), cellSize(cellSize), offset(offset)
{
    moved = LoadSound("sounds/move-self.wav");
    capture = LoadSound("sounds/capture.wav");
    gameend = LoadSound("sound/game-end.wav");
    isWhiteMov = true;
    selectedPiece = nullptr;
    dragging = false;
    gameOver = false;
    whiteCastle = false;
    blackCastle = false;
    isKingside = false;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            board[row][col] = Piece();
        }
    }
    LoadTextures();
    InitializePieces();
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
    // Undo on backspace button press (unchanged)
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        Undo();
    }
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
                offset + column * cellSize,
                offset + row * cellSize,
                cellSize, cellSize, cellColor);
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
            static_cast<float>(mousePos.x - cellSize / 2), 
            static_cast<float>(mousePos.y - cellSize / 2), 
            static_cast<float>(cellSize - 20), 
            static_cast<float>(cellSize - 20) 
        };
        Vector2 origin = { 0, 0 };

        DrawTexturePro(selectedPiece->getTexture(), sourceRec, destRec, origin, 0.0f, WHITE);
    }
}

void Board::Undo()
{
    if (moveHistory.empty()) return;

    // Remove last move from history vector
    MoveHistory lastMove = moveHistory.back();
    moveHistory.pop_back();

    // Update the position to previous one on the board
    SetPiece(lastMove.startY, lastMove.startX, lastMove.movedPiece);
    if (lastMove.capturedPiece.id != 0) {
        SetPiece(lastMove.endY, lastMove.endX, lastMove.capturedPiece);
    } else {
        SetPiece(lastMove.endY, lastMove.endX);
    }

    // Update castling flags after undoing the move
    whiteKingMoved = lastMove.prevWhiteKingMoved;
    blackKingMoved = lastMove.prevBlackKingMoved;
    whiteKingSideRookMoved = lastMove.prevWhiteKingsideRookMoved;
    whiteQueenSideRookMoved = lastMove.prevWhiteQueensideRookMoved;
    blackKingSideRookMoved = lastMove.prevBlackKingsideRookMoved;
    blackQueenSideRookMoved = lastMove.prevBlackQueensideRookMoved;

    // Handle if move was castling move
    if (lastMove.wasCastlingMove) {
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
    }
    if (lastMove.movedPiece.id < 0) {  
        whiteCastle = false;
    } else {  
        blackCastle = false;
    }

    // Play sound and switch turn
    PlaySound(capture);
    switchTurn();
}

void Board::handleMove()
{
    mousePos = GetMousePosition();
    int mouseX = (mousePos.x - offset) / cellSize;
    int mouseY = (mousePos.y - offset) / cellSize;

    // Check if mouse is within board bounds
    if (mouseX >= 0 && mouseX < 8 && mouseY >= 0 && mouseY < 8)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Piece &clickedPiece = board[mouseY][mouseX];

            if (clickedPiece.id != 0) // Check if a piece exists
            {
                // Validate piece color turn
                bool isWhitePiece = clickedPiece.id < 0;
                if ((isWhitePiece && isWhiteMov) || (!isWhitePiece && !isWhiteMov))
                {
                    selectedPiece = &clickedPiece;
                    originalRow = mouseY;
                    originalCol = mouseX;
                    dragging = true;
                }
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
                
                // Validate move based on piece rules
                if (!selectedPiece->IsValidMove(originalCol, originalRow, mouseX, mouseY, *this))
                {
                    selectedPiece->SetPosition(originalCol, originalRow);
                    dragging = false;
                    selectedPiece = nullptr;
                    return;
                }
                
                // Simulate move to check for self-check
                Piece tempBoard[8][8];
                memcpy(tempBoard, board, sizeof(board));

                // Simulate moves 
                Piece movedPiece = *selectedPiece;
                Piece capturedPiece = GetPiece(mouseY, mouseX);

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
                ExecuteMove(movedPiece, mouseX, mouseY);

                switchTurn();

                bool isCurrentCheckmate = IsCheckmate(isWhiteMov);
                // Handle checkmate
                if (isCurrentCheckmate)
                {
                    PlaySound(gameend);
                    printf("%s is in checkmate!\n", isWhiteMov ? "Black" : "White");
                    printf("Game Over\n");
                    gameOver = true; // Set gameover flag true
                    return;
                }
                else if (IsInCheck(isWhiteMov))
                {
                    printf("%s is in check!\n", isWhiteMov ? "White" : "Black");
                }
                
                playMoveSound(mouseX, mouseY, isCurrentCheckmate, capturedPiece.id != 0);

                dragging = false;
                selectedPiece = nullptr;
            }
        }
    }
    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        dragging = false;
        selectedPiece = nullptr;
    }
}

void Board::switchTurn()
{
    isWhiteMov = !isWhiteMov;
}

bool Board::IsCheckmate(bool whiteKing) const {
    if (!IsInCheck(whiteKing))
        return false;

    int kingRow, kingCol;
    if (!FindKingPosition(whiteKing, kingRow, kingCol))
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
                const Piece& destPiece = board[newRow][newCol];
                if (destPiece.id * kingId > 0)
                    continue;

                // Create a non-const copy for move simulation
                simpleBoard tempBoard = static_cast<const simpleBoard&>(*this);
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
            const Piece& piece = board[fromRow][fromCol];
            if (piece.id == 0) continue;

            bool isPieceWhite = piece.id < 0;
            if (isPieceWhite != whiteKing) continue;

            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    // Create a non-const copy for move simulation
                    simpleBoard tempBoard = static_cast<const simpleBoard&>(*this);
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
    }

    else 
    {   
        // Normal moves
        movedPiece.SetPosition(endX, endY);
        board[originalRow][originalCol] = Piece();
        board[endY][endX] = movedPiece;

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

// Play move sounds
void Board::playMoveSound(int endX, int endY, bool isCurrentCheckmate, bool isCapture)
{
    if(isCurrentCheckmate) PlaySound(gameend);
    else if (isCapture) PlaySound(capture);
    else PlaySound(moved);
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
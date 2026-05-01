#include "gameStateManager.h"
#include "fenGenerator.h"

GameStateManager::GameStateManager()  
{
    casteling = false;
    whiteCastle =false;
    blackCastle = false;
    whiteKingMoved = false;
    blackKingMoved = false;
    whiteQueenSideRookMoved = false;
    whiteKingSideRookMoved = false;
    blackKingSideRookMoved = false;
    blackQueenSideRookMoved = false;
}

bool GameStateManager::IsInCheck(bool whiteKing) const
{
    int kingRow, kingCol;
    if (!FindKingPosition(whiteKing, kingRow, kingCol))
        return false;
    return IsSquareUnderAttack(kingRow, kingCol, !whiteKing);
}

bool GameStateManager::FindKingPosition(bool whiteKing, int &kingRow, int &kingCol) const
{
    int targetId = whiteKing ? -5 : 5;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            if (board[row][col].id == targetId)
            {
                kingRow = row;
                kingCol = col;
                return true;
            }
        }
    }
    return false;
}

bool GameStateManager::IsSquareUnderAttack(int row, int col, bool attackedByWhite) const
{
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            Piece attacker = board[r][c];
            if (attacker.id == 0)
                continue;

            // Check if attacker is of opposite color
            bool isOpponentPiece = attackedByWhite ? (attacker.id < 0) : (attacker.id > 0);
            if (isOpponentPiece)
            {
                if (attacker.IsValidMove(c, r, col, row, *this))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

const Piece& GameStateManager::GetPiece(int row, int col) const 
{
    static const Piece emptyPiece;  // Static empty piece for invalid coordinates
    if (row >= 0 && row < 8 && col >= 0 && col < 8) 
    {
        return board[row][col];
    }
    return emptyPiece;
}

bool GameStateManager::HasRookMoved(bool isWhite, bool isKingside) const 
{
    if (isWhite) 
    {
        return isKingside ? whiteKingSideRookMoved : whiteQueenSideRookMoved;
    } else 
    {
        return isKingside ? blackKingSideRookMoved : blackQueenSideRookMoved;
    }
}

bool GameStateManager::HasKingMoved(bool isWhite) const 
{
    return isWhite ? whiteKingMoved : blackKingMoved;
}

// Check if castling attempt is valid
void GameStateManager::checkCastelingAttempt(int startX, int startY, int endX, int endY, const Piece* piece)
{
    casteling = false;
    if (!piece || piece->id == 0) return;
    
    // Check casteling attempted
    if (!IsInCheck(isWhiteMov) && (std::abs(piece->id) == 5 && ((isWhiteMov && !whiteCastle) || (!isWhiteMov && !blackCastle)))) // King
    {
        if(endY == startY && ((isWhiteMov && startY == 7) || (!isWhiteMov && startY == 0))) {
            if (startX - endX == 2) {isKingside = false, casteling = true;}
            else if (endX - startX == 2) {isKingside = true, casteling = true;}
        }
    }

    // Check if king or participating rook moved earlier
    if(HasKingMoved(isWhiteMov)) {casteling = false; return;}
    if(HasRookMoved(isWhiteMov, isKingside)) {casteling = false; return;}
}

void GameStateManager::simulateCasteling(Piece &movedPiece, int startX, int startY, int endX, int endY) 
{
    if((isWhiteMov && whiteCastle) || (!isWhiteMov && blackCastle)) 
    {
        casteling = false;
        return; // Player already did casteling
    }

    // Move king for simulation
    movedPiece.SetPosition(endX, endY);
    board[startY][startX] = Piece();
    board[endY][endX] = movedPiece;
    
    int rookFromCol = (isKingside) ? 7 : 0;
    int rookToCol = (isKingside) ? (endX - 1) : (endX + 1);

    // Simulate the rook movement
    if (endY >= 0 && endY < 8 && rookFromCol >= 0 && rookFromCol < 8) {
        Piece rook = board[endY][rookFromCol];
        board[endY][rookFromCol] = Piece();
        if (rookToCol >= 0 && rookToCol < 8) {
            board[endY][rookToCol] = rook;
        }
    }
}

// Execute the move
void GameStateManager::ExecuteMove(Piece& movedPiece, int startX, int startY, int endX, int endY) 
{
    Piece capturedPiece = GetPiece(endY, endX);

    MoveHistory move;
    move.startX = startX;
    move.startY = startY;
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
        board[startY][startX] = Piece();
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
        // Pawn promotion: move pawn to promotion square
        // Actual piece replacement handled by Board (requires textures)
        movedPiece.SetPosition(endX, endY);
        board[startY][startX] = Piece();
        board[endY][endX] = movedPiece;
        
        move.wasPawnPromotion = true;
        Piece capturedPiece = GetPiece(endY, endX);
        move.capturedPiece = capturedPiece;
        isPawnPromotion = false;
        halfMovesCounter = 0;
    }

    else if(enPassantCapture)
    {
        if (abs(movedPiece.id) == 6 && enPassantCol == endX && enPassantRow == endY && board[endY][endX].id == 0) {
            int capturedPawnRow = (movedPiece.id < 0) ? endY + 1 : endY - 1;
            move.capturedPiece = GetPiece(capturedPawnRow, endX);
            board[capturedPawnRow][endX] = Piece(); // Remove the captured pawn
            // Move the pawn to the destination
            movedPiece.SetPosition(endX, endY);
            board[startY][startX] = Piece();
            board[endY][endX] = movedPiece;
            enPassant = false;
            enPassantCapture = false;
            move.wasEnPassant = true;
            move.enPassantCapturedRow = (movedPiece.id < 0) ? endY + 1 : endY - 1;
            move.enPassantCapturedCol = endX;
            halfMovesCounter = 0;
        }
    }

    else 
    {   
        // Normal moves
        movedPiece.SetPosition(endX, endY);
        board[startY][startX] = Piece();
        board[endY][endX] = movedPiece;
        enPassant = false;
        enPassantCapture = false;

        // check for en passant if this is a pawn move
        if (abs(movedPiece.id) == 6) 
        {
            // Check for pawn double move
            if (abs(endY - startY) == 2) 
            {
                enPassantCol = endX;
                enPassantRow = (startY + endY) / 2;
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
        
        if(!whiteQueenSideRookMoved && id == -1 && startY == 7 && startX == 0) whiteQueenSideRookMoved = true;
        else if(!whiteKingSideRookMoved && id == -1 && startY == 7 && startX == 7) whiteKingSideRookMoved = true;
        else if(!blackQueenSideRookMoved && id == 1 && startY == 0 && startX == 0) blackQueenSideRookMoved = true;
        else if(!blackKingSideRookMoved && id == 1 && startY == 0 && startX == 7) blackKingSideRookMoved = true;
    }
    moveHistory.push_back(move);
}

void GameStateManager::checkPawnPromotion(int endX, int endY, const Piece* piece) 
{
    if (!piece) return;
    int id = piece->id;
    if(id != 6 && id != -6) return;

    int targetRow = isWhiteMov ? 0 : 7;
    if(endY != targetRow) return;

    isPawnPromotion = true;
}

void GameStateManager::switchTurn()
{
    isWhiteMov = !isWhiteMov;
}

void GameStateManager::Undo()
{
    if (moveHistory.empty()) return;

    // Remove last move from history vector
    if (moveHistory.back().movedPiece.id > 0) moveHistory.pop_back();
    MoveHistory lastMove = moveHistory.back();
    moveHistory.pop_back();

    // Remove from past board history
    if (!boardHistory.empty()) boardHistory.pop();

    // Update position history (FEN generation handled by derived Board class)
    // positionHistory[fenGenerator::generateFEN(*this)]--;

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

    // Switch turn (sound will be handled by Board)
    switchTurn();
}

void GameStateManager::SetPiece(int row, int col, Piece& piece)
{
    piece.SetPosition(col, row);
    board[row][col] = piece;
}

void GameStateManager::SetPiece(int row, int col)
{
    board[row][col] = Piece();
}

void GameStateManager::saveState()
{
    std::vector<Piece> snapshot;
    snapshot.reserve(64);
    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
            snapshot.push_back(board[row][col]);
    boardHistory.push(snapshot);
}

// Validate and execute move - pure chess logic without UI
bool GameStateManager::ValidateAndExecuteMove(int fromX, int fromY, int toX, int toY, bool isWhiteTurn, Piece* selectedPiece)
{
    if (!selectedPiece || selectedPiece->id == 0)
        return false;

    // Prevent moving opponent's pieces
    if ((selectedPiece->id < 0 && !isWhiteTurn) || (selectedPiece->id > 0 && isWhiteTurn))
        return false;

    // Check for castling attempt
    checkCastelingAttempt(fromX, fromY, toX, toY, selectedPiece);

    // Check for pawn promotion attempt
    checkPawnPromotion(toX, toY, selectedPiece);

    // Validate move based on piece rules
    if (!selectedPiece->IsValidMove(fromX, fromY, toX, toY, *this))
        return false;

    // Simulate move to check for self-check
    Piece tempBoard[8][8];
    memcpy(tempBoard, board, sizeof(board));

    // Simulate moves
    Piece movedPiece = *selectedPiece;

    // Simulate castling if flagged
    if (casteling)
    {
        movedPiece.SetPosition(toX, toY);
        int rookFromCol, rookToCol;
        if (isKingside) { rookFromCol = 7; rookToCol = toX - 1; }
        else { rookFromCol = 0; rookToCol = toX + 1; }

        Piece rook = board[toY][rookFromCol];
        rook.SetPosition(rookToCol, toY);
        board[toY][rookFromCol] = Piece();
        board[toY][rookToCol] = rook;
        board[fromY][fromX] = Piece();
        board[toY][toX] = movedPiece;
    }
    else
    {
        // Normal moves
        movedPiece.SetPosition(toX, toY);
        board[fromY][fromX] = Piece();
        board[toY][toX] = movedPiece;
    }

    // Check if move puts own king in check
    bool kingInCheck = IsInCheck(isWhiteTurn);

    if (kingInCheck)
    {
        selectedPiece->SetPosition(fromX, fromY);
        casteling = false;
        memcpy(board, tempBoard, sizeof(board));
        return false;
    }

    // Restore board to previous state (will be executed properly by Board::ExecuteMove)
    memcpy(board, tempBoard, sizeof(board));

    return true;  // Move is valid, Board::ExecuteMove will handle actual execution
}
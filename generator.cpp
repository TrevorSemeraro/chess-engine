#include <vector>

#include "pch.h"

namespace MoveGenerator
{
    const int knight_directions[8][2] = {
    {2, 1},
    {2, -1},
    {-2, 1},
    {-2, -1},
    {1, 2},
    {1, -2},
    {-1, 2},
    {-1, -2} };

    const int bishop_directions[4][2] = {
    {1, 1},
    {1, -1},
    {-1, 1},
    {-1, -1}
    };

    const int rook_directions[4][2] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1} };

    void updateAttackedSquaresAndPossibleMoves(Board::Board& board)
	{
		for (int i = 0; i < 64; i++)
		{
			board.white_attacked_squares[i] = false;
			board.black_attacked_squares[i] = false;
		}

        if (board.whiteTurn) {
            updateBlackMoves(board);
            updateWhiteMoves(board);
        }
		else {
			updateWhiteMoves(board);
			updateBlackMoves(board);
		}
	}

    void updateWhiteMoves(Board::Board& board)
	{
		board.white_moves.clear();
		for (Board::Piece& p : board.white_pieces)
		{
			if (p.type != ' ')
			{
				getPossibleMoves(p, board, board.white_moves);
			}
		}
	}

    void updateBlackMoves(Board::Board& board)
    {
        board.black_moves.clear();
        for (Board::Piece& p : board.black_pieces)
        {
            if (p.type != ' ')
            {
                getPossibleMoves(p, board, board.black_moves);
            }
        }
    }

    void getAllPossibleMoves(Board::Board& board, std::vector<Board::Move>& moves)
    {
        moves.clear();
        bool whiteTurn = board.whiteTurn;

        Board::Piece king = whiteTurn ? board.white_pieces[0] : board.black_pieces[0];
        auto opponentAttackedSquares = whiteTurn ? board.black_attacked_squares: board.white_attacked_squares;

        if (opponentAttackedSquares[Board::PositionToIndex(king.position)]) {
            for (int i = 0; i < 16; i++)
            {
                Board::Piece p = whiteTurn ? board.white_pieces[i] : board.black_pieces[i];

                if (p.type != ' ')
                    getPossibleMoves(p, board, moves);
            }
        }
        else {
            for (int i = 0; i < 16; i++)
            {
                Board::Piece p = whiteTurn ? board.white_pieces[i] : board.black_pieces[i];

                if (p.type != ' ')
                    getPossibleMoves(p, board, moves);
            }
        }

        if (whiteTurn) {
            board.white_moves = moves;
        }
        else {
            board.black_moves = moves;
        }
    }

    void getPossibleMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves) {
        piece.checkDirection = Board::MovementDirection::None;
        char type = tolower(piece.type);

        bool isKingInCheck = board.isKingInCheck(piece.white);

        // for non-king pieces, we will save the direction in which they are seeing the opposing king
        // when we are in check, loop over opposing pieces, get the pin directions.
        Board::Piece(*opposingPieces)[16] = piece.white ? &board.black_pieces : &board.white_pieces;

        int attackers = 0;
        // attackers == 0, there is no check, generate moves as typical
        
        // attackers == 1, there is a check, but we can capture
        // or block IF attacker is not a knight
        
        // attackers == 2, there are multiple checks, we can only move king
        Board::Piece* attackingPiece = nullptr;
        Board::MovementDirection attackingDirection = Board::MovementDirection::None;

        for (Board::Piece& opposing : *opposingPieces) {
            if (opposing.type == ' ') continue;

            if (opposing.checkDirection != Board::MovementDirection::None) {
                attackers += 1;
                attackingDirection = opposing.checkDirection;

                if (attackingPiece == nullptr) {
                    attackingPiece = &opposing;
				}
            }
        }

        if (attackers == 2) {
            // if we are in check, and there are two attackers, we can only move the king
			if (type != 'k') {
				return;
			}
        }

        //std::cout << piece.white << isKingInCheck << " - King is in check by " << attackers << " attackers." << std::endl;

        //Board::Piece* kingPiece = &board.white_pieces[0];

        // if attackingPiece != nullptr then we are in check with a singular piece
        switch (type) {
        case 'p':
        {
            getPawnMoves(piece, board, moves, attackingPiece);
            break;
        }
        case 'n':
        {
            getKnightMoves(piece, board, moves, attackingPiece);
            break;
        }
        case 'b':
        {
            getBishopMoves(piece, board, moves, attackingPiece);
            break;
        }
        case 'r':
        {
            getRookMoves(piece, board, moves, attackingPiece);
            break;
        }
        case 'q':
        {
            getRookMoves(piece, board, moves, attackingPiece);
            getBishopMoves(piece, board, moves, attackingPiece);
            break;
        }
        case 'k':
        {
            getKingMoves(piece, board, moves, attackingPiece);
            break;
        }
        default:
        {
            break;
        }
        }
    }

    void getKingMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& validMoves, Board::Piece* attacker) {
        bool inCheck = (attacker != nullptr);

        for (uint8_t x = 0; x < 3; x++)
        {
            for (uint8_t y = 0; y < 3; y++)
            {
                int newRow = piece.position.row + x - 1;
                int newCol = piece.position.col + y - 1;
                Board::Position newPosition = { newRow, newCol };

                if (inBounds(newPosition) && !(newPosition == piece.position))
                {
                    int position = PositionToIndex(newPosition);
                    bool isWhitePiece = isupper(board.board[position]) > 0;

                    auto opponentAttackedSquares = piece.white ? board.black_attacked_squares : board.white_attacked_squares;
                    auto selfAttackedSquares = piece.white ? board.white_attacked_squares : board.black_attacked_squares;

                    bool isOpponentAttacking = opponentAttackedSquares[position];

                    if (isOpponentAttacking) continue;

                    selfAttackedSquares[position] = true;

                    if (board.board[position] == ' ')
                    {
                        Board::Move move = { piece.type, piece.position, newPosition, false, ' ', ' ', false, Board::Castle::CastleType::None};
                        validMoves.push_back(move);
                    }
                    if (isWhitePiece != piece.white)
                    {
                        Board::Move move = { piece.type, piece.position, newPosition, true, board.board[position], ' ', false, Board::Castle::CastleType::None };
                        validMoves.push_back(move);
                    }
                }
            }
        }

        // if we are in check, disable castling
        if(inCheck) return;
        
        int row = piece.white ? 7 : 0;
        Board::Position necessaryKingPosition = { row, 4 };
        
        bool kingInPosition = piece.position == necessaryKingPosition;
        if (!kingInPosition) return;

        if (piece.white) {

            bool isKingsideOpen = (board.board[Board::PositionToIndex(row, 5)] == ' ') && (board.board[Board::PositionToIndex(row, 6)] == ' ');
            bool isQueensideOpen = (board.board[Board::PositionToIndex(row, 1)] == ' ') && (board.board[Board::PositionToIndex(row, 2)] == ' ') && (board.board[Board::PositionToIndex(row, 3)] == ' ');

            if(board.canWhiteCastleKing && isKingsideOpen)
			{
                Board::Move move = { piece.type, piece.position, {row, 6}, false, ' ', ' ', false, Board::Castle::CastleType::Kingside };
                validMoves.push_back(move);
			}

            if (board.canWhiteCastleQueen && isQueensideOpen)
            {
                Board::Move move = { piece.type, piece.position, {row, 2}, false, ' ', ' ', false, Board::Castle::CastleType::Queenside };
                validMoves.push_back(move);
            }
        }
        else {
            bool isKingsideOpen = board.board[Board::PositionToIndex(row, 5)] == ' ' && board.board[Board::PositionToIndex(row, 6)] == ' ';
            bool isQueensideOpen = board.board[Board::PositionToIndex(row, 1)] == ' ' && board.board[Board::PositionToIndex(row, 2)] == ' ' && board.board[Board::PositionToIndex(row, 3)] == ' ';

            if (board.canBlackCastleKing && isKingsideOpen)
            {
                Board::Move move = { piece.type, piece.position, {row, 6}, false, ' ', ' ', false, Board::Castle::CastleType::Kingside };
                validMoves.push_back(move);
            }

            if (board.canBlackCastleQueen && isQueensideOpen)
            {
                Board::Move move = { piece.type, piece.position, {row, 2}, false, ' ', ' ', false, Board::Castle::CastleType::Queenside };
                validMoves.push_back(move);
            }
        }
    }

    bool filterCheckMoves(Board::Piece& piece, Board::Board& board, Board::Piece* attacker, Board::Move move) {
        Board::MovementDirection direction = attacker->checkDirection;
        // UP_RIGHT = 1, -1
        int* change = getScalarMovementByMovementDirection(direction);

        Board::Piece& king = piece.white ? board.white_pieces[0] : board.black_pieces[0];

        double rowScalar = change[0];
        double colScalar = change[1];

        bool betweenPieces = Board::betweenPositions(king.position, attacker->position, move.finalPosition);

        // ensure that the move is between the attacker and the king
        if (betweenPieces) {
            int rowDiff = attacker->position.row - move.finalPosition.row;
            int colDiff = attacker->position.col - move.finalPosition.col;

            //std::cout << "Piece to attacker position difference: " << +pieceToAttackerPositionDifference.row << ", " << +pieceToAttackerPositionDifference.col << std::endl;

            // if the row/col scalar is 0, and the row/col difference is not 0, we cannot move
            // i.e. if the direction of the attack is up/down, we must move to a position that is up/down from the attacker
            if (rowScalar == 0 && rowDiff != 0) return false;
            if (colScalar == 0 && colDiff != 0) return false;

            // show that there exists an x s.t. x * rowScalar = rowDiff, x * colScalar = colDiff
            // t.e. x = rowDiff / rowScalar = colDiff / colScalar

            if (rowScalar != 0 && colScalar != 0) {
                double rowX = rowDiff / rowScalar;
                double colX = colDiff / colScalar;

                // if the x values are equal, we can move to the position
                if (rowX != colX) return false;
            }

            return true;
            //moves.push_back(move);
        }

        return false;
    }


    void getKnightMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves, Board::Piece* attacker)
    {
        Board::Piece& king = piece.white ? board.white_pieces[0] : board.black_pieces[0];
        std::vector<Board::Move> validMoves;

        bool white = piece.white;
        auto attackedSquares = white ? board.white_attacked_squares : board.black_attacked_squares;
        
        // if the knight is pinned, it has no valid moves
        if (piece.isPinned) {
            //std::cout << "Knight is pinned, cannot move" << std::endl;
            return;
        }

        for (auto combination : knight_directions)
        {
            int newx = piece.position.row + combination[0];
            int newy = piece.position.col + combination[1];
            Board::Position newPosition = { newx, newy };

            if (inBounds(newPosition))
            {

                int position = PositionToIndex(newPosition);
                
                attackedSquares[position] = true;

                bool isWhitePiece = isupper(board.board[position]) > 0;

                bool canCapture = white != isWhitePiece;
                bool isEmpty = board.board[position] == ' ';

                if (isEmpty || canCapture)
                {
                    Board::Move move = { piece.type, piece.position, newPosition, canCapture, board.board[position] };
                    validMoves.push_back(move);
                }
            }
        }

        if (attacker != nullptr){
            for (Board::Move& move : validMoves) {
                bool validMove = filterCheckMoves(piece, board, attacker, move);

                if (validMove) {
                    moves.push_back(move);
                }
            }
        }
        else {
            moves.insert(moves.end(), validMoves.begin(), validMoves.end());
        }
    }

    void getPawnMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves, Board::Piece* attacker) {
        bool inCheck = (attacker != nullptr);
        bool white = piece.white;

        auto attackedSquares = white ? board.white_attacked_squares : board.black_attacked_squares;

        int pawnDirection = white ? -1 : 1;
        int pawnHome = white ? 6 : 1;
        int oppositePawnHome = white ? 1 : 6;

        int row = piece.position.row;
        int col = piece.position.col;

        char promotionChar = white ? 'Q' : 'q';

        std::vector<Board::Move> validMoves;

        // push
        if (!piece.isPinned || piece.pinnedDirection == Board::MovementDirection::UP_DOWN) {
            int pawnPush = Board::PositionToIndex(row + pawnDirection, col);
            if (board.board[pawnPush] == ' ' && row + pawnDirection < 8 && row + pawnDirection >= 0)
            {
                if (row + pawnDirection == 0 || row + pawnDirection == 7)
                {
                    Board::Move move = { piece.type, row, col, row + pawnDirection, col, false, ' ', promotionChar };
                    validMoves.push_back(move);
                }
                else
                {
                    Board::Move move = { piece.type, row, col, row + pawnDirection, col };
                    validMoves.push_back(move);
                }
            }

            // double push, if non obstructed
            int pawnDoublePush = Board::PositionToIndex(row + pawnDirection * 2, col);
            if (row == pawnHome && board.board[pawnDoublePush] == ' ' && board.board[pawnPush] == ' ')
            {
                Board::Move move = { piece.type, row, col, row + pawnDirection * 2, col };
                validMoves.push_back(move);
            }
        }

        // captures
        if (!piece.isPinned && row + pawnDirection >= 0 && row + pawnDirection <= 7)
        {
            // can take going north west
            int CaptureLeft = Board::PositionToIndex(row + pawnDirection, col - 1);
            int CaptureRight = Board::PositionToIndex(row + pawnDirection, col + 1);

            attackedSquares[CaptureLeft] = true;
            attackedSquares[CaptureRight] = true;

            char capturedLeft = board.board[CaptureLeft];
            char capturedRight = board.board[CaptureRight];

            bool canCaptureLeft = capturedLeft != ' ' && white != isupper(capturedLeft) > 0;
            bool canCaptureRight = capturedRight != ' ' && white != isupper(capturedRight) > 0;

            char opposingKing = white ? 'k' : 'K';

            if (col + 1 <= 7 && canCaptureRight)
            {
                // TODO: set king in check
                //if (board.board[CaptureRight] == opposingKing) {
                //    Board::Move move = { piece.type, row, col, row + pawnDirection, col + 1, true, board.board[CaptureRight], promotionChar };
                //    validMoves.push_back(move);
                //}

                if (row + pawnDirection == 0 || row + pawnDirection == 7)
                {
                    Board::Move move = { piece.type, row, col, row + pawnDirection, col + 1, true, board.board[CaptureRight], promotionChar };
                    validMoves.push_back(move);
                }
                else {
                    Board::Move move = { piece.type, row, col, row + pawnDirection, col + 1, true, board.board[CaptureRight]};
                    validMoves.push_back(move);
                }
            }
            if (col - 1 >= 0 && canCaptureLeft)
            {
                if (row + pawnDirection == 0 || row + pawnDirection == 7)
                {
                    Board::Move move = { piece.type, row, col, row + pawnDirection, col - 1, true, board.board[CaptureLeft], promotionChar };
                    validMoves.push_back(move);
                }
                else {
                    Board::Move move = { piece.type, row, col, row + pawnDirection, col - 1, true, board.board[CaptureLeft] };
                    validMoves.push_back(move);
                }
            }
        }

        // enpassant
        if (!piece.isPinned && board.pastMoves.size() > 0)
        {
            Board::Move lastMove = board.pastMoves.back();
            char lastPiece = tolower(lastMove.piece);
            
            bool lastMovePawn = lastPiece == 'p';

            bool lastMoveDoublePush = abs(lastMove.initalPosition.row - lastMove.finalPosition.row) == 2;

            int moverow = lastMove.finalPosition.row;
            int movecol = lastMove.finalPosition.col;
            
            int colDiff = abs(col - movecol);

            if (lastMovePawn && lastMoveDoublePush && moverow == row && colDiff == 1)
            {
                if (col > movecol)
                {
                    Board::Move move = { piece.type, row, col, row + pawnDirection, col - 1, true, 'p', ' ', true};
                    validMoves.push_back(move);
                }
                else
                {
                    Board::Move move = { piece.type, row, col, row + pawnDirection, col + 1, true, 'p', ' ', true };
                    validMoves.push_back(move);
                }
            }
        }

        if (attacker != nullptr) {
            for (Board::Move& move : validMoves) {
                bool validMove = filterCheckMoves(piece, board, attacker, move);

                if (validMove) {
                    moves.push_back(move);
                }
            }
        }
        else {
            moves.insert(moves.end(), validMoves.begin(), validMoves.end());
        }
    }

    void getBishopMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves, Board::Piece* attacker)
    {
        bool inCheck = (attacker != nullptr);
        bool white = piece.white;

        auto attackedSquares = white ? board.white_attacked_squares : board.black_attacked_squares;

        uint8_t row = piece.position.row;
        uint8_t col = piece.position.col;

        // pinned in direction we cannot move.
        if(piece.isPinned){
            if (piece.pinnedDirection == Board::MovementDirection::UP_DOWN ||
                piece.pinnedDirection == Board::MovementDirection::LEFT_RIGHT){
                return;
            }
        }

        std::vector<Board::Move> validMoves;

        for (auto direction : bishop_directions)
        {
            int rowInc = direction[0];
            int colInc = direction[1];

            int piecesSeen = 0;
            Board::Piece* seenPiece = nullptr;

            Board::MovementDirection direction = Board::MovementDirection::None;

            // convert the direction to a pin direction
            if (rowInc == 1) {
				if (colInc == 1) {
					direction = Board::MovementDirection::UP_LEFT_DOWN_RIGHT;
				}
				else {
					direction = Board::MovementDirection::UP_RIGHT_DOWN_LEFT;
				}
			}
			else {
				if (colInc == 1) {
					direction = Board::MovementDirection::UP_RIGHT_DOWN_LEFT;
				}
				else {
					direction = Board::MovementDirection::UP_LEFT_DOWN_RIGHT;
				}
			}

            // if the piece is pinned, and we are NOT pinned in this direction, we cannot move
            if(piece.isPinned && piece.pinnedDirection != direction){
				continue;
			}

            for (uint8_t x = row, y = col; x < 8 && y < 8 && x >= 0 && y >= 0; x += rowInc, y += colInc)
            {
                if (x == row && y == col)
                {
                    continue;
                }

                Board::Position newPosition = { x, y };
                int newPositionIndex = Board::PositionToIndex(newPosition);

                // if we haven't seen a piece, or we have seen a king, we will mark the square as attacked
                if (seenPiece == nullptr || tolower(seenPiece->type) == 'k') {
                    attackedSquares[newPositionIndex] = true;
                }

                if (board.board[newPositionIndex] != ' ')
                {
                    try {
                        Board::Piece& current = board.getPieceAtPosition(newPosition);
                        bool isCurrentKing = current.type == (piece.white ? 'k' : 'K');

                        // "xray" to check if a piece is pinned to king, and also prevent king from moving into check
                        if (isCurrentKing) {
                            if(piecesSeen == 0){
                                //std::cout << piece.type << " setting check direction to " << direction << std::endl;
							    piece.checkDirection = direction;
						    }

                            if (seenPiece != nullptr && seenPiece->white != piece.white) {
                                //std::cout << "Current piece is king, seen piece is " << seenPiece->type << std::endl;
                                //std::cout << "Pinning piece " << seenPiece->type << " at " << +seenPiece->position.row << ", " << +seenPiece->position.col << std::endl;
                            
                                seenPiece->isPinned = true;
                                seenPiece->pinnedDirection = direction;

                                piece.pinningPiece = seenPiece;
                                piece.pinningDirection = direction;
                            }
                        }

                        seenPiece = &current;

                        // if we have already seen a piece, and are looking at another piece, we will exit
                        if (piecesSeen >= 1) {
                            break;
                        }
                        piecesSeen++;

                        // if we can capture piece, it is a valid move
                        bool isWhitePiece = isupper(board.board[newPositionIndex]) > 0;
                        if (white != isWhitePiece)
                        {
                            // check if piece is king
                            Board::Move move = { piece.type, piece.position, newPosition, true, board.board[newPositionIndex] };
                            validMoves.push_back(move);
                        }
                    }
                    catch (...) {
                        std::cout << "Error getting piece at position " << +newPosition.row << ", " << +newPosition.col << std::endl;
                        for (Board::Move& move : board.pastMoves) {
                            printMove(move);
                        }
                    }
                }
                else {
                    // we can only move there if there isn't a piece in the way
                    if (piecesSeen == 0) {
                        Board::Move move = { piece.type, piece.position, newPosition };
                        validMoves.push_back(move);
                    }
                }
            }
        }

        if (attacker != nullptr) {
            for (Board::Move& move : validMoves) {
                bool validMove = filterCheckMoves(piece, board, attacker, move);

                if (validMove) {
                    moves.push_back(move);
                }
            }
        }
        else {
            moves.insert(moves.end(), validMoves.begin(), validMoves.end());
        }
    }

    void getRookMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves, Board::Piece* attacker)
    {
        bool inCheck = (attacker != nullptr);
        bool white = piece.white;

        auto attackedSquares = white ? board.white_attacked_squares : board.black_attacked_squares;
        int row = piece.position.row;
        int col = piece.position.col;

        if (piece.isPinned) {
            if (piece.pinnedDirection == Board::MovementDirection::UP_RIGHT_DOWN_LEFT ||
                piece.pinnedDirection == Board::MovementDirection::UP_LEFT_DOWN_RIGHT){
                return;
            }
        }

        std::vector<Board::Move> validMoves;

        for (auto direction : rook_directions)
        {
            int xinc = direction[0];
            int yinc = direction[1];

            int piecesSeen = 0;
            Board::Piece* seenPiece = nullptr;

            Board::MovementDirection moveDirection = Board::MovementDirection::None;

            // convert the direction to a pin direction
            if (yinc == 1 || yinc == -1) {
                moveDirection = Board::MovementDirection::LEFT_RIGHT;
            }
            if(xinc == 1 || xinc == -1){
                moveDirection = Board::MovementDirection::UP_DOWN;
            }

            // if the piece is pinned, and we are NOT pinned in the current scoping direction, return
            bool pinnedInDirection = piece.pinnedDirection == moveDirection;
            if (piece.isPinned && !pinnedInDirection) {
                //std::cout << piece.type << " is pinned by << " << piece.pinnedBy << " " << piece.pinnedBy->type << " cannot move in " << piece.pinnedDirection << std::endl;
                continue;
            }

            for (uint8_t x = row, y = col; x < 8 && y < 8 && x >= 0 && y >= 0; x += xinc, y += yinc)
            {
                if (x == row && y == col)
                {
                    continue;
                }

                Board::Position newPosition = { x, y };
                int newPositionIndex = Board::PositionToIndex(newPosition);

                // if we haven't seen a piece, or we have seen a king, we will mark the square as attacked
                if (seenPiece == nullptr || tolower(seenPiece->type) == 'k') {
                    attackedSquares[newPositionIndex] = true;
                }

                if (board.board[newPositionIndex] != ' ')
                {
				    Board::Piece& current = board.getPieceAtPosition(newPosition);
                    bool isCurrentKing = current.type == (piece.white ? 'k' : 'K');
                    
                    // "xray" to check if a piece is pinned to king, and also prevent king from moving into check
                    if (isCurrentKing) {
                        if (piecesSeen == 0) {
                            //std::cout << piece.type << " setting check direction to " << moveDirection << std::endl;
                            piece.checkDirection = moveDirection;
                        }

                        if (seenPiece != nullptr && seenPiece->white != piece.white) {
                            seenPiece->isPinned = true;
                            seenPiece->pinnedDirection = moveDirection;
                            seenPiece->pinnedBy = &piece;
                            
                            piece.pinningPiece = seenPiece;
                            
                            //std::cout << piece.type << " " << &piece << " is pinning " << piece.pinningPiece->type << " " << piece.pinningPiece << " at " << +piece.pinningPiece->position.row << ", " << +piece.pinningPiece->position.col << std::endl;
                            piece.pinningDirection = moveDirection;
                        }
                    }
                    else {
                        seenPiece = &current;
                    }

                    // if we have already seen a piece, and are looking at another piece, we will exit
                    if (piecesSeen >= 1) {
                        break;
                    }
                    piecesSeen++;

                    // if we can capture piece, it is a valid move
                    bool isWhitePiece = isupper(board.board[newPositionIndex]) > 0;
                    if (white != isWhitePiece)
                    {
                        // check if piece is king
                        Board::Move move = { piece.type, piece.position, newPosition, true, board.board[newPositionIndex] };
                        validMoves.push_back(move);
                    }
                }
                else {
                    // we can only move there if there isn't a piece in the way
                    if (piecesSeen == 0) {
                        Board::Move move = { piece.type, piece.position, newPosition };
                        validMoves.push_back(move);
                    }
                }
            }
        }

        if (attacker != nullptr) {
            for (Board::Move& move : validMoves) {
                bool validMove = filterCheckMoves(piece, board, attacker, move);

                if (validMove) {
                    moves.push_back(move);
                }
            }
        }
        else {
            moves.insert(moves.end(), validMoves.begin(), validMoves.end());
        }
    }
}
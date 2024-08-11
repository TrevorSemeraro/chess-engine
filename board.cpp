#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <cstdint>

#include "pch.h"

namespace Board
{
    Board::Board()
    {
        // Inital white turn
        this->whiteTurn = true;
        this->whiteTimer.Start();
        this->blackTimer.Reset();
        this->blackTimer.Pause();

        this->moves = {};
        this->numberOfMoves = 0;

        this->currentEvaluation = Evaluate::evaluateBoard(*this);
    }

    Board::~Board()
    {
        // Destructor
    }

    void Board::move(Move move, bool print)
    {
        //std::cout << "Initial Zobrist Hash: " << this->zobristKey << std::endl;
        if (print) printMove(move);

        int initalposition = PositionToIndex(move.initalPosition.row, move.initalPosition.col);
        int finalposition = PositionToIndex(move.finalPosition.row, move.finalPosition.col);

        char piece = move.piece;
        char pieceType = tolower(piece);
        bool isWhitePiece = isupper(move.piece) > 0;

        this->board[initalposition] = ' ';

        Piece(*currentPieces)[16] = isWhitePiece ? &this->white_pieces : &this->black_pieces;
        Piece(*opposingPieces)[16] = isWhitePiece ? &this->black_pieces : &this->white_pieces;

        bool isCastling = move.castle != 0;
        bool isPromoting = move.promotion != ' ';

        int currentPieceIndex = -1;
        int index = 0;
        for (Piece& current : *currentPieces)
        {
            if (current.position == move.initalPosition)
            {
                current.position = move.finalPosition;

                currentPieceIndex = index;
            }
            index++;
        }
        movedPieceIndex = currentPieceIndex;
        Piece* currentPiece = &(*currentPieces)[currentPieceIndex];

        if (move.willCapture) {
            this->handleCapture(move, opposingPieces, isWhitePiece);
        }
        if (isCastling) {
            this->handleCastle(move, currentPieces, isWhitePiece, print);
        }
        else {
            this->board[finalposition] = piece;
        }

        if (isPromoting) {
            this->handlePromotion(move, currentPieces, currentPieceIndex, isWhitePiece, print);
        }
        else {
            this->board[finalposition] = piece;
        }

        if (pieceType == 'k') {
            Position kingPositionChange = getScalarPositionChange(move.initalPosition, move.finalPosition);

            MovementDirection kingMovement = getDirection(kingPositionChange);

            //std::cout << "King Movement: " << kingMovement << std::endl;

            // for all pieces, if there is a pin, get the pin direction
            for (Piece& opposingPiece : *opposingPieces) {

                if (opposingPiece.type == ' ') continue;

                // if the pin does not follow the movement of the king
                bool pinFollowKing = opposingPiece.pinningDirection == kingMovement;

                if (opposingPiece.pinningPiece != nullptr) {
                    if (pinFollowKing) {
                        //std::cout << "Keeping pin" << std::endl;
                    }
                    else {
                        //std::cout << "Removing pin on " << opposingPiece.pinningPiece->type << std::endl;
                        
                        opposingPiece.pinningPiece->isPinned = false;
                        opposingPiece.pinningPiece->pinnedDirection = MovementDirection::None;

                        opposingPiece.pinningPiece = nullptr;
                        opposingPiece.pinningDirection = MovementDirection::None;
                    }
                }
            }
        }

        if (currentPiece->pinningPiece != nullptr) {
            Position positionChange = getScalarPositionChange(move.initalPosition, move.finalPosition);
            MovementDirection moveDirection = getDirection(positionChange);

            MovementDirection pinningDirection = currentPiece->pinningDirection;

            if (moveDirection != pinningDirection) {
                currentPiece->pinningPiece->isPinned = false;
                currentPiece->pinningPiece->pinnedDirection = MovementDirection::None;

                currentPiece->pinningPiece = nullptr;
                currentPiece->pinningDirection = MovementDirection::None;
            }
            else {
                //std::cout << "Could NOT remove pin on " << currentPiece->pinningPiece->type << std::endl;
            }
        }

        // update the check direction for all opposing pieces
        // since we moved, we cannot be in check anymore
        for (Piece& p : *opposingPieces) {
			p.checkDirection = MovementDirection::None;
		}

        this->flipTimer();

        try{
            if(print)
                std::cout << "There are " << this->moves.size() << " moves" << std::endl;
            this->moves.push_back(move);
        }
        catch (...) {
            std::cout << "Error" << std::endl;
        }
        this->numberOfMoves++;
        
        MoveGenerator::updateAttackedSquaresAndPossibleMoves(*this);

        this->whiteTurn = !this->whiteTurn;

        bool gameDraw = this->checkDraw(this->whiteTurn);
        if (gameDraw) {
            //std::cout << "Stalemate." << std::endl;
            this->gameState = GameState::Draw;
            return;
        }

        bool gameOver = this->isInCheckmate(this->whiteTurn);
        if (gameOver) {
            //std::cout << "Checkmate." << std::endl;
            this->gameState = GameState::Checkmate;
            this->whiteWon = !this->whiteTurn;
            return;
        }

        this->currentEvaluation = Evaluate::evaluateBoard(*this);
        this->zobristKey = zobrist::updateHash(*this, move);

        //std::cout << "New Zobrist Hash: " << this->zobristKey << std::endl;
    }

    void Board::handleCastle(Move& move, Piece(*currentPieces)[16], bool isWhitePiece, bool print) {
        // kingside castle
        if (print) {
            //std::cout << "Castling" << std::endl;
        }

        uint8_t castleRow = isWhitePiece ? 7 : 0;

        Position kingPosition;
        Position rookPosition;
        Position startingRookPosition;

        if (move.castle == 1) {
            kingPosition = Position{ castleRow, 6 };
            rookPosition = Position{ castleRow, 5 };

            startingRookPosition = Position{ castleRow, 7 };
        }
        // queenside castle
        else {
            kingPosition = Position{ castleRow, 2 };
            rookPosition = Position{ castleRow, 3 };

            startingRookPosition = Position{ castleRow, 0 };
        }

        for (Piece& p : *currentPieces)
        {
            char type = tolower(p.type);

            if (type == 'k') {
                p.position = kingPosition;
            }
            if (type == 'r' && p.position == startingRookPosition) {
                this->board[PositionToIndex(startingRookPosition)] = ' ';
                this->board[PositionToIndex(rookPosition)] = isWhitePiece ? 'R' : 'r';
                p.position = rookPosition;
            }
        }

        if (isWhitePiece) {
            this->canWhiteCastleKing = false;
            this->canWhiteCastleQueen = false;
        }
        else {
            this->canBlackCastleKing = false;
            this->canBlackCastleQueen = false;
        }
    }

    void Board::undoMove(Move move) {
        this->whiteTurn = !this->whiteTurn;

        int movedFrom = PositionToIndex(move.initalPosition.row, move.initalPosition.col);
        int movedTo = PositionToIndex(move.finalPosition.row, move.finalPosition.col);

        bool undoEnpassant = move.enPassant;
        bool undoPromotion = move.promotion != ' ';
        bool undoCapture = move.willCapture;
        
        char piece = move.piece;
        bool isKingMovement = tolower(piece) == 'k';

        if (movedPieceIndex != -1) {
            Piece(*currentPieces)[16] = this->whiteTurn ? &this->white_pieces : &this->black_pieces;
            Piece movedPiece = (*currentPieces)[movedPieceIndex];

            movedPiece.position = move.initalPosition;
        }
        else {
            throw new std::runtime_error("Moved piece not found");
        }

        this->board[PositionToIndex(move.initalPosition)] = piece;
        this->board[PositionToIndex(move.finalPosition)] = ' ';

        if(undoEnpassant) {
		}


        if (undoPromotion) {
            
        }

        if (undoCapture) {
            if (capturedPieceIndex != -1) {
                this->board[PositionToIndex(move.finalPosition)] = capturedPiece.type;
                
                Piece(*opposingPieces)[16] = this->whiteTurn ? &this->black_pieces : &this->white_pieces;
				(*opposingPieces)[capturedPieceIndex] = capturedPiece;
            }
            else {
                throw new std::runtime_error("Captured piece not found");
            }
        }


    }

    void Board::handlePromotion(Move& move, Piece(*currentPieces)[16], int currentPieceIndex, bool isWhitePiece, bool print) {
        int pawnFinalRow = isWhitePiece ? 0 : 7;
        
        if (tolower(move.piece) == 'p' && move.finalPosition.row == pawnFinalRow)
        {
            if (currentPieceIndex > 0 && currentPieceIndex < 16) {
                currentPieces[currentPieceIndex]->type = move.promotion;
            }

            if (print) {
                std::cout << "Promoting pawn to " << move.promotion << std::endl;
            }
            this->board[PositionToIndex(move.finalPosition)] = move.promotion;
        }
    }

    void Board::handleCapture(Move& move, Piece (*opposingPieces)[16], bool isWhitePiece) {
        // remove the piece in the row one from the opposite pawn direction 
        Position capturePosition = move.finalPosition;
        
        if (move.enPassant) {
            // white pawns are moving in the negative direction
            uint8_t enpassantRow = isWhitePiece ? move.finalPosition.row + 1 : move.finalPosition.row - 1;

            capturePosition = { enpassantRow, move.finalPosition.col };
        }

        // we are going to capture (remove) any opposing piece on the final positions square
        for (int i = 0; i < 16; i++)
        {
            Piece& opposingPiece = *opposingPieces[i];
            capturedPieceIndex = i;

            if (opposingPiece.position == capturePosition)
            {
                opposingPiece.type = ' ';
                opposingPiece.position = { 0, 0 };

                break;
            }
        }

        this->board[PositionToIndex(capturePosition)] = ' ';
    }

    void Board::flipTimer() {
        if (this->whiteTurn)
        {
            this->whiteTimer.Pause();
            this->blackTimer.Start();
        }
        else
        {
            this->blackTimer.Pause();
            this->whiteTimer.Start();
        }
    }

    void Board::loadState(std::string fen)
    {
        // board, turn, castling, enpassant
        std::vector<std::string> inputs = {};

        std::string tmp = "";
        for (const char c : fen)
        {
            if (c == ' ')
            {
                inputs.push_back(tmp);
                tmp = "";
            }
            else
            {
                tmp += c;
            }
        }
        inputs.push_back(tmp);

        std::string board_state = inputs[0];
        uint8_t row = 0;
        uint8_t col = 0;

        int kingIndex = 0;
        int whitePieceIndex = kingIndex + 1;
        int blackPieceIndex = kingIndex + 1;

        for (int i = 0; i < board_state.size(); i++)
        {
            char c = board_state[i];

            if (c == '/')
            {
                row++;
                col = 0;

                continue;
            }
            else
            {
                if (isdigit(c))
                {
                    int diff = c - '0';
                    for (int i = 0; i < diff; i++)
                    {
                        int position = PositionToIndex(row, col);
                        board[position] = ' ';
                        col++;
                    }
                }
                else
                {
                    bool isWhitePiece = isupper(c) > 0;
                    Position pos = { row, col };
                    Piece* current = new Piece(pos, c, isWhitePiece);
                
                    if (isWhitePiece)
                    {
                        if (c == 'K')
                        {
                            this->white_pieces[kingIndex] = *current;
                        }
                        else
                            this->white_pieces[whitePieceIndex++] = *current;
                    }
                    else
                    {
                        if (c == 'k') {
                            this->black_pieces[kingIndex] = *current;
                        }
                        else {
                            this->black_pieces[blackPieceIndex++] = *current;
                        }
                    }

                    int position = PositionToIndex(row, col);
                    board[position] = c;

                    col++;
                }
            }
        }

        while (whitePieceIndex < 16) {
            Piece* current = new Piece({0, 0}, ' ', true);
            this->white_pieces[whitePieceIndex++] = *current;
        }

        while (blackPieceIndex < 16) {
            Piece* current = new Piece({ 0, 0 }, ' ', false);
            this->black_pieces[blackPieceIndex++] = *current;
        }

        // TURN
        if (inputs.size() > 1)
        {
            std::string turn = inputs[1];
            this->whiteTurn = turn == "w";
        }

        // CASTLING
        if (inputs.size() > 2)
        {
            std::string castling = inputs[2];

            for (const char c : castling)
            {
                switch (c) {
                    case 'K':
						this->canWhiteCastleKing = true;
						break;
					case 'Q':
						this->canWhiteCastleQueen = true;
						break;
					case 'k':
						this->canBlackCastleKing = true;
						break;
					case 'q':
						this->canBlackCastleQueen = true;
						break;
				
                }
            }
        }

        if (inputs.size() > 3)
        {
            std::string enpassant = inputs[3];

            if (enpassant.size() >= 2)
            {
                char file = enpassant[0];
                char rank = enpassant[1];

                // if its white turn, the possible enpassant came from black on row 7 (6),
                // if its black turn, the possible enpassant came from white on row 2 (1)
                int inital = this->whiteTurn ? 6 : 1;
                char piecetype = this->whiteTurn ? 'p' : 'P';
                Position pos = { inital, file - 'a' };

                Move m = { piecetype, inital, file - 'a', rank - '0', file - 'a' };

                this->moves.push_back(m);
            }
        }

        //std::cout << "Loaded board state" << std::endl;

        MoveGenerator::updateAttackedSquaresAndPossibleMoves(*this);
        this->currentEvaluation = Evaluate::evaluateBoard(*this);
        this->zobristKey = zobrist::generateHash(*this);
    }

    void Board::print()
    {
        std::cout << "====================" << std::endl;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                int position = PositionToIndex(i, j);
                std::cout << board[position] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "====================" << std::endl;
        std::cout << "White Pieces" << std::endl;
        for (const Piece &p : this->white_pieces)
        {
            if (p.type == ' ') continue;
            std::cout << p.type << " ";
        }

        std::cout << std::endl << "====================" << std::endl;
        std::cout << "Black Pieces" << std::endl;
        for (const Piece &p : this->black_pieces)
        {
            if (p.type == ' ') continue;
            std::cout << p.type << " ";
        }
        std::cout << std::endl << "====================" << std::endl;

    }

    bool Board::isKingInCheck(bool isWhite)
	{
        if (isWhite) {
            for (Piece &p : this->white_pieces)
			{
				if (p.type == 'K') {
					if (this->black_attacked_squares[PositionToIndex(p.position)])
					{
						return true;
					}
				}
			}
		}
		else {
			for (Piece &p : this->black_pieces)
			{
				if (p.type == 'k') {
					if (this->white_attacked_squares[PositionToIndex(p.position)])
					{
						return true;
					}
				}
			}
        }
        return false;
	}

    // true if there is a draw
    bool Board::checkDraw(bool white)
	{
        std::vector<Move> colorMoves = white ? this->white_moves : this->black_moves;

        // 50 move rule
        // TODO: implement

        // threefold repetition
        // todo : implement

        // insufficient material
        // todo : implement

        // stalemate
        if (!this->isKingInCheck(white)) {
            return colorMoves.size() == 0;
        }

        return false;
	}

    // returns true if the color provided is in checkmate
    bool Board::isInCheckmate(bool white) {
        std::vector<Move> colorMoves = white ? this->white_moves : this->black_moves;

        // white king is in in check
        if (this->isKingInCheck(white)) {
            return colorMoves.size() == 0;
        }

        return false;
    }

    // Returns a nullptr if a piece is not located
    Piece& Board::getPieceAtPosition(Position position) {
        for (int i = 0; i < 16; i++)
        {
            if (this->white_pieces[i].position == position)
            {
                return this->white_pieces[i];
            }
            if (this->black_pieces[i].position == position)
            {
                return this->black_pieces[i];
            }
        }
        throw std::runtime_error("Piece not found");
    }
}

#pragma once
#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <vector>
#include <string>
#include <string>

#include "Position.h"
#include "Move.h"
#include "timer.h"
#include "Piece.h"

namespace Board
{
    enum GameState {
        Loading=0,
        Playing=1,
        Checkmate=2,
        Draw=3
    };

    class Board {
    public:
        GameState gameState = GameState::Loading;
        bool whiteWon = false;

        bool whiteTurn;

        uint64_t zobristKey = 0;

        std::vector<Move> moves = {};
        int numberOfMoves = 0;

        double currentEvaluation = 0;

        Piece white_pieces[16] = {};
        Piece black_pieces[16] = {};
        
        // the follow are used so we can undo moves
        Piece capturedPiece = {};
        int capturedPieceIndex = -1;
        int movedPieceIndex = -1;

        CustomTimer whiteTimer;
        CustomTimer blackTimer;

        // restrict king movement outside of these squares, if the king is in one of these squares, we detect check, and dont allow castling
        bool black_attacked_squares[64] = { false };
        bool white_attacked_squares[64] = { false };

        // pieces that are putting opposite king in check
        // SAVE 0 for the KING
        Piece blackAttackingPieces[16] = {};
        Piece whiteAttackingPieces[16] = {};

        Piece& getPieceAtPosition(Position position);

        std::vector<Move> white_moves = {};
        std::vector<Move> black_moves = {};


        char board[64] = {
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        };

        // Constructor
        Board();
        void loadState(std::string fen);

        void print();

        void move(Move move, bool print);
        void undoMove(Move move);

        void handleCapture(Move& move, Piece(*opposingPieces)[16], bool isWhitePiece);
        void handlePromotion(Move& move, Piece(*currentPieces)[16], int currentPieceIndex, bool isWhitePiece, bool print);
        void handleCastle(Move& move, Piece(*currentPieces)[16], bool isWhitePiece, bool print);

        void flipTimer();

        bool isKingInCheck(bool white);
        bool checkDraw(bool white);
        bool isInCheckmate(bool white);

        // Destructor
        ~Board();

        bool canWhiteCastleKing = false;
        bool canWhiteCastleQueen = false;

        bool canBlackCastleKing = false;
        bool canBlackCastleQueen = false;
    };
}

#endif // BOARD_H
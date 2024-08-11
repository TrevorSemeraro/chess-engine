#pragma once

#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <unordered_map>

#include "Position.h"
#include "Move.h"
#include "board.h"
#include "movement.h"

namespace Board
{
    const static int PAWN_VALUE = 100;
    const static int KNIGHT_VALUE = 325;
    const static int BISHOP_VALUE = 325;
    const static int ROOK_VALUE = 500;
    const static int QUEEN_VALUE = 1025;
    const static int KING_VALUE = 40000;

    // Piece Types
    const enum PieceType : int {
        None = 0,
        Pawn = 1,
        Knight = 2,
        Bishop = 3,
        Rook = 4,
        Queen = 5,
        King = 6,
    };

    // Piece Colours
    const enum PieceColour : int {
		White = 0,
		Black = 8,
	};

    const int WhitePawn = Pawn | White; // 1
    const int WhiteKnight = Knight | White; // 2
    const int WhiteBishop = Bishop | White; // 3
    const int WhiteRook = Rook | White; // 4
    const int WhiteQueen = Queen | White; // 5
    const int WhiteKing = King | White; // 6

    const int BlackPawn = Pawn | Black; // 9
    const int BlackKnight = Knight | Black; // 10
    const int BlackBishop = Bishop | Black; // 11
    const int BlackRook = Rook | Black; // 12
    const int BlackQueen = Queen | Black; // 13
    const int BlackKing = King | Black; // 14

    const int MaxPieceIndex = BlackKing;

    static int PieceIndices[] = {
        WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing,
        BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing
    };

    // Bit Masks
    const int typeMask = 0b0111;
    const int colourMask = 0b1000;

    class Piece {
    public:
        int piece;
        bool white;

        Position position;

        Movement::Direction checkDirection = Movement::Direction::None;

        Movement::Direction pinningDirection = Movement::Direction::None;
        Piece* pinningPiece = nullptr;
        Piece* pinnedBy = nullptr;
        
        bool isPinned = false;
        Movement::Direction pinnedDirection = Movement::Direction::None;

        //std::vector<Move> moves;

        Piece();
        Piece(Position pos, PieceType pieceType, PieceColour white);

        Piece(Position pos, PieceType pieceType, bool white) {
            Piece(pos, pieceType, white ? PieceColour::White : PieceColour::Black);
        }
    
        bool isWhite() {
            return (piece & colourMask) == White && piece != 0;
        }
     };

    int CreatePiece(int pieceType, int pieceColour);

    int CreatePiece(int pieceType, bool isWhite);

    PieceColour GetPieceColour(int piece);

    PieceType GetPieceType(int piece);

    extern const std::unordered_map<int, int> pieceValues;

    PieceType charToPieceType(char c);
}


#endif
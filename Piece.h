#pragma once

#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <unordered_map>

#include "Position.h"
#include "Move.h"
#include "board.h"

namespace Board
{
    const static int PAWN_VALUE = 100;
    const static int KNIGHT_VALUE = 325;
    const static int BISHOP_VALUE = 325;
    const static int ROOK_VALUE = 500;
    const static int QUEEN_VALUE = 1025;
    const static int KING_VALUE = 40000;

    const enum PieceIndex {
        WHITE_PAWN = 0,
        WHITE_ROOK,
        WHITE_KNIGHT,
        WHITE_BISHOP,
        WHITE_QUEEN,
        WHITE_KING,
        BLACK_PAWN,
        BLACK_ROOK,
        BLACK_KNIGHT,
        BLACK_BISHOP,
        BLACK_QUEEN,
        BLACK_KING = 11,
    };

    enum MovementDirection {
        None = 0,
		UP_DOWN = 1,
		LEFT_RIGHT = 2,
		UP_LEFT_DOWN_RIGHT = 3,
		UP_RIGHT_DOWN_LEFT = 4,
        KNIGHT = 5,
	};

    int* getScalarMovementByMovementDirection(MovementDirection direction);

    Position getScalarPositionChange(Position start, Position end);
    MovementDirection getDirection(Position change);

    class Piece {
    public:
        char type;
        bool white;

        Position position;

        MovementDirection checkDirection = MovementDirection::None;

        MovementDirection pinningDirection = MovementDirection::None;
        Piece* pinningPiece = nullptr;
        Piece* pinnedBy = nullptr;
        
        bool isPinned = false;
        MovementDirection pinnedDirection = MovementDirection::None;

        //std::vector<Move> moves;

        Piece();
        Piece(Position pos, char type, bool white);
    };

    extern const std::unordered_map<char, int> pieceValues;
}


#endif
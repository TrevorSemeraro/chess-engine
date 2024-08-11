#pragma once
#ifndef MOVE_H
#define MOVE_H

#include <vector>

#include "Position.h"
#include "Piece.h"

namespace Board
{
    namespace Castle {
        enum CastleType {
            None = 0,
            Kingside = 1,
            Queenside = 2,
            Both = 3
        };
    }

    struct Move {
        Piece piece;
        bool isWhite;
        
        Position initalPosition;
        Position finalPosition;

        bool willCapture = false;

        int captured = PieceType::None;
        PieceType promotion = PieceType::None;

        bool enPassant = false;

        Castle::CastleType castle = Castle::CastleType::None;
    };
    void printMove(Move move);
}


#endif
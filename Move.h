#pragma once
#ifndef MOVE_H
#define MOVE_H

#include <vector>

#include "Position.h"

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
        char piece;
        
        Position initalPosition;
        Position finalPosition;

        bool willCapture = false;

        char captured = ' ';

        char promotion = ' ';

        bool enPassant = false;

        // 0 = no castle, 1 = kingside castle, 2 = queenside castle
        Castle::CastleType castle = Castle::CastleType::None;
    };
    void printMove(Move move);
}


#endif
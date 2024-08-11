#pragma once
#ifndef MOVE_H
#define MOVE_H

#include <vector>

#include "Position.h"

namespace Board
{
    struct Move {
        char piece;
        
        Position initalPosition;
        Position finalPosition;

        bool willCapture = false;

        char captured = ' ';

        char promotion = ' ';

        bool enPassant = false;

        // 0 = no castle, 1 = kingside castle, 2 = queenside castle
        int castle = 0;
    };
    void printMove(Move move);
}


#endif
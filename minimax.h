#pragma once

#ifndef MINIMAX_H
#define MINIMAX_H

#include <vector>
#include "board.h"
#include "Move.h"
#include "zobrist.h"

namespace MoveGenerator
{
    extern int leafCount;
    extern int hitCount;

    Board::zobrist::MoveScore findOptimalMove(Board::Board* board, bool isAIWhite);
    int minimax(Board::Board* board, int playsRemaining, int playsFromRoot, int alpha, int beta);
    bool moveSorter(const Board::Move& a, const Board::Move& b);
}

#endif // MINIMAX_H
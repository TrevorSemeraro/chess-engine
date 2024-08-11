#pragma once
#pragma once
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "board.h"

namespace Evaluate
{
	extern const int Pawns[64];
	extern const int PawnsEnd[64];
	extern const int Rooks[64];
	extern const int Knights[64];
	extern const int Bishops[64];
	extern const int Queens[64];
	extern const int Kings[64];

	int evaluateBoard(Board::Board& board);
	int MaterialScore(Board::Board& board);
	int PieceSquareScore(Board::Board& board);
}

#endif
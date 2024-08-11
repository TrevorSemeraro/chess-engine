#pragma once

#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <unordered_map>
#include <random>

#include "board.h"
#include "Move.h"
#include "minimax.h"

namespace Board
{
	namespace zobrist
	{
		enum Flag {
			NOTFOUND = -1,
			EXACT = 0,
			LOWERBOUND = 1,
			UPPERBOUND = 2
		};
		
		struct MoveScore {
			int evaluation;
			int playsFromRoot;
			Flag flag;
			Move move;
		};

		extern std::unordered_map<uint64_t, MoveScore> transpositionTable;

		extern uint64_t whiteToMoveKey;
		extern uint64_t castlingKeys[16];
		extern uint64_t pieceKeys[2][6][64];
		extern uint64_t enPassantKeys[8];
		extern uint64_t blackToMove;

		//const unsigned int seed = 12345;

		void init();

		uint64_t generateHash(Board& board);
		uint64_t updateHash(Board& board, Move& move);

		void StoreState(uint64_t state, MoveScore score);

		MoveScore GetState(uint64_t state, int playsRemaining, int playsFromRoot, int alpha, int beta);
	}
}
#endif // ZOBRIST_H

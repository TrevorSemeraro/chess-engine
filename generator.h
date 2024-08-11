#pragma once

#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>

#include "Piece.h"
#include "board.h"
#include "Move.h"

namespace MoveGenerator
{
	extern const int knight_directions[8][2];
	extern const int bishop_directions[4][2];
	extern const int rook_directions[4][2];

	void updateAttackedSquaresAndPossibleMoves(Board::Board& board);
	void updateWhiteMoves(Board::Board& board);
	void updateBlackMoves(Board::Board& board);

	void getPossibleMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves);
	void getAllPossibleMoves(Board::Board& board, std::vector<Board::Move>& moves);

	void getKingMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves, Board::Piece* attacker);
	void getKnightMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves, Board::Piece* attacker);
	void getPawnMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves, Board::Piece* attacker);
	void getBishopMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves, Board::Piece* attacker);
	void getRookMoves(Board::Piece& piece, Board::Board& board, std::vector<Board::Move>& moves, Board::Piece* attacker);
}

#endif
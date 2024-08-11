#include <vector>
#include <unordered_map>

#include "pch.h"

namespace Board
{
	int* getScalarMovementByMovementDirection(MovementDirection direction) {
		switch (direction) {
		case UP_DOWN:
			return new int[2] { 1, 0 };
		case LEFT_RIGHT:
			return new int[2] { 0, 1 };
		case UP_LEFT_DOWN_RIGHT:
			return new int[2] { 1, 1 };
		case UP_RIGHT_DOWN_LEFT:
			return new int[2] { 1, -1 };
		case KNIGHT:
			// there is no scalar movement for knight
			return new int[2] { 0, 0 };
		default:
			return new int[2] { 0, 0 };
		}
	}

	Position getScalarPositionChange(Position start, Position end) {
		// xf - xi, yf - yi divided by positive self
		// 5, -5 => 5/5, -5/5 => 1, -1
		int xDiff = end.col - start.col;
		int yDiff = end.row - start.row;

		xDiff = xDiff == 0 ? 0 : xDiff / abs(xDiff);
		yDiff = yDiff == 0 ? 0 : yDiff / abs(yDiff);

		Position change = { yDiff, xDiff };
		return change;
	}

	MovementDirection getDirection(Position change) {
		if (change.row == 0 && change.col == 0) {
			return None;
		}
		if (change.row == 0) {
			return LEFT_RIGHT;
		}
		if (change.col == 0) {
			return UP_DOWN;
		}
		if (change.row == change.col) {
			return UP_LEFT_DOWN_RIGHT;
		}
		if (change.row == -change.col) {
			return UP_RIGHT_DOWN_LEFT;
		}
		return None;
	}

	const std::unordered_map<char, int> pieceValues = {
		{'P', PAWN_VALUE},
		{'R', ROOK_VALUE},
		{'N', KNIGHT_VALUE},
		{'B', BISHOP_VALUE},
		{'Q', QUEEN_VALUE},
		{'K', KING_VALUE},
		{'p', -PAWN_VALUE},
		{'r', -ROOK_VALUE},
		{'n', -KNIGHT_VALUE},
		{'b', -BISHOP_VALUE},
		{'q', -QUEEN_VALUE},
		{'k', -KING_VALUE},
		{' ', 0}
	};

	Piece::Piece() {
		this->position = { 0, 0 };
		this->type = ' ';
		this->white = false;
	}
	Piece::Piece(Position pos, char type, bool white) {
		this->position = pos;
		this->type = type;
		this->white = white;
	}
}

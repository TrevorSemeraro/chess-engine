#include "pch.h"

namespace Board
{
	namespace Movement
	{
		int* getScalarMovementByMovementDirection(Direction direction) {
			switch (direction) {
			case Direction::UP_DOWN:
				return new int[2] { 1, 0 };
			case Direction::LEFT_RIGHT:
				return new int[2] { 0, 1 };
			case Direction::UP_LEFT_DOWN_RIGHT:
				return new int[2] { 1, 1 };
			case Direction::UP_RIGHT_DOWN_LEFT:
				return new int[2] { 1, -1 };
			case Direction::KNIGHT:
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

		Direction getDirection(Position change) {
			if (change.row == 0 && change.col == 0) {
				return Direction::None;
			}
			if (change.row == 0) {
				return Direction::LEFT_RIGHT;
			}
			if (change.col == 0) {
				return Direction::UP_DOWN;
			}
			if (change.row == change.col) {
				return Direction::UP_LEFT_DOWN_RIGHT;
			}
			if (change.row == -change.col) {
				return Direction::UP_RIGHT_DOWN_LEFT;
			}
			return Direction::None;
		}
	}
}
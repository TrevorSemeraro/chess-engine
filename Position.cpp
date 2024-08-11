#include <cstdint>
#include <unordered_map>

#include "pch.h"

namespace Board
{
    bool operator==(const Position& lhs, const Position& rhs) {
        return lhs.row == rhs.row && lhs.col == rhs.col;
    }
    Position operator+(const Position& lhs, const Position& rhs) {
        uint8_t row = lhs.row + rhs.row;
        uint8_t col = lhs.col + rhs.col;

        return { row, col };
    }
    Position operator-(const Position& lhs, const Position& rhs) {
        uint8_t row = lhs.row - rhs.row;
        uint8_t col = lhs.col - rhs.col;

        return { row, col };
    }
    bool betweenPositions(Position start, Position end, Position test) {
        uint8_t rowMin = std::min(start.row, end.row);
        uint8_t rowMax = std::max(start.row, end.row);
        uint8_t colMin = std::min(start.col, end.col);
        uint8_t colMax = std::max(start.col, end.col);

        return test.row >= rowMin && test.row <= rowMax && test.col >= colMin && test.col <= colMax;
    }

    bool inBounds(Position pos) {
        return pos.row >= 0 && pos.row < 8 && pos.col >= 0 && pos.col < 8;
    }

    int PositionToIndex(int row, int col) {
        return row * 8 + col;
    }

    int PositionToIndex(Position p) {
        return p.row * 8 + p.col;
    }
}

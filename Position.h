#pragma once
#include <cstdint>

#ifndef POSITION_H
#define POSITION_H

namespace Board
{
    struct Position {
        uint8_t row;
        uint8_t col;
    };

    bool operator==(const Position& lhs, const Position& rhs);
    Position operator+(const Position& lhs, const Position& rhs);
    Position operator-(const Position& lhs, const Position& rhs);
    bool betweenPositions(Position start, Position end, Position test);

    int PositionToIndex(Position p);
    int PositionToIndex(int row, int col);
    bool inBounds(Position pos);
}

#endif#pragma once

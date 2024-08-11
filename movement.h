#pragma once
#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Position.h"

namespace Board {
    namespace Movement {
        enum Direction {
            None = 0,
            UP_DOWN = 1,
            LEFT_RIGHT = 2,
            UP_LEFT_DOWN_RIGHT = 3,
            UP_RIGHT_DOWN_LEFT = 4,
            KNIGHT = 5,
        };

        int* getScalarMovementByMovementDirection(Direction direction);

        Position getScalarPositionChange(Position start, Position end);
        Direction getDirection(Position change);
    }
}

#endif
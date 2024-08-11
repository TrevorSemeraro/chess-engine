// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <cstdint>
#include <chrono>
#include <vector>

// add headers that you want to pre-compile here
#include "board.h"
#include "Evaluator.h"
#include "framework.h"
#include "minimax.h"
#include "Move.h"
#include "generator.h"
#include "Piece.h"
#include "Position.h"
#include "timer.h"
#include "zobrist.h"

#endif //PCH_H

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <chrono>

#include "pch.h"

#define MAX_DEPTH 2
#define INITAL_ALPHA -1000000
#define INITAL_BETA 1000000

namespace MoveGenerator
{
    int leafCount = 0;
    int hitCount = 0;
    
    const int MATE_SCORE = 1000000;
    const int MAX_ITTERATIVE_DEPTH = 15;

    bool isComputerMaximizingPlayer = false;

    std::chrono::steady_clock::time_point start;
    double maxDuration = 60.0 * 1000;

    Board::zobrist::MoveScore bestMove = {};
    Board::zobrist::MoveScore bestMoveThisItteration = {};

    Board::zobrist::MoveScore findOptimalMove(Board::Board* board, bool isAIWhite) {
        std::cout << "Finding optimal move for " << (isAIWhite ? "white" : "black") << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        // iterative deeping?
        bool maximizingPlayer = isAIWhite;
        int startingEval = maximizingPlayer ? -1000000 : 1000000;

        for (int i = 0; i < MAX_ITTERATIVE_DEPTH; i++) {
            int evaluation = minimax(board, MAX_DEPTH, 0, INITAL_ALPHA, INITAL_BETA);
        }

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout  << duration.count() << std::endl;

        std::cout << "Leaf count: " << leafCount << std::endl;
        std::cout << "Hit count: " << hitCount << std::endl;
        std::cout << "Eval: " << evaluation << std::endl;

        return bestMove;
    }

    int minimax(Board::Board* board, int playsRemaining, int playsFromRoot, int alpha, int beta) {
        leafCount++;

        // Transposition table lookup
        if (Board::zobrist::transpositionTable.find(board->zobristKey) != Board::zobrist::transpositionTable.end()) {
            hitCount++;

            Board::zobrist::MoveScore eval = Board::zobrist::transpositionTable.at(board->zobristKey);
            
            // If the same position has already been searched to at least an equal depth
            // to the search we're doing now,we can just use the recorded evaluation.
            if (eval.playsFromRoot >= playsFromRoot) {
                bestMove = eval;
                return eval.evaluation;
            }
        }

        // base case exit condition
        if(playsRemaining == 0) {
			int evaluation = Evaluate::evaluateBoard(*board);
            return evaluation;
		}

        // minimax algorithm below
        std::vector<Board::Move>& moves = board->whiteTurn ? board->white_moves : board->black_moves;

        std::sort(moves.begin(), moves.end(), &moveSorter);

        if (moves.size() == 0) {
            bool checkmate = board->isKingInCheck(board->whiteTurn);

            if (checkmate) {
				int score = board->whiteTurn ? -MATE_SCORE : MATE_SCORE;
				return score;
			}
            else {
                return 0;
            }
        }

        for (const Board::Move& move : moves) {
            Board::Board* newBoard = new Board::Board(*board);

            newBoard->move(move, false);

            int eval = minimax(newBoard, playsRemaining - 1, playsFromRoot + 1, alpha, beta);

            Board::zobrist::MoveScore score = { eval, playsFromRoot + 1, Board::zobrist::EXACT, move };

            Board::zobrist::StoreState(newBoard->zobristKey, score);

            if (eval >= beta) {
                return beta;
            }

            if(eval > alpha) {
				alpha = eval;
                bestMoveThisItteration = score;

                if (playsFromRoot == 0) {
                    bestMove = score;
                }
			}

        }

        return alpha;
    }

    bool moveSorter(const Board::Move& a, const Board::Move& b) {
        bool positionedBefore = (a.initalPosition.row != b.initalPosition.row) ? a.initalPosition.row < b.initalPosition.row : a.initalPosition.col < b.initalPosition.col;

        int aVal = abs(Board::pieceValues.at(tolower(a.piece)));
        int bVal = abs(Board::pieceValues.at(tolower(b.piece)));

        if (a.willCapture && b.willCapture) {

            int aCaptureVal = abs(Board::pieceValues.at(tolower(a.captured)));
            int bCaptureVal = abs(Board::pieceValues.at(tolower(b.captured)));

            if (aCaptureVal != bCaptureVal) {
                return aCaptureVal > bCaptureVal;
            }

            if (aVal != bVal) {
                return aVal < bVal;
            }

            return positionedBefore;
        }

        if (a.willCapture) return true;
        if (b.willCapture) return false;

        if (aVal != bVal) {
            return aVal < bVal;
        }

        return positionedBefore;
    }
}


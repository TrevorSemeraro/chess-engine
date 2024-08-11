#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <chrono>

#include "pch.h"

#define INITAL_ALPHA -100000000
#define INITAL_BETA 100000000
#define MICROSECONDS_PER_SECOND 1000000
#define MAX_ITTERATIVE_DEPTH 5
#define MATE_SCORE 1000000

/// <summary>
/// Iterative deepening, using a transposition table, embed the depth-first algorithms like alpha-beta into a framework with best-first characteristics.
/// </summary>
namespace MoveGenerator
{
    int leafCount = 0;
    int hitCount = 0;
    
    bool isComputerMaximizingPlayer = false;

    std::chrono::steady_clock::time_point start;
    double maxDuration = 60.0 * MICROSECONDS_PER_SECOND;

    bool searchCancelled = false;
    bool hasSearchedAtLeastOneMove = false;
    int currentItterationDepth = 0;

    Board::zobrist::MoveScore bestMove = {};
    Board::zobrist::MoveScore bestMoveThisItteration = {};

    Board::zobrist::MoveScore findOptimalMove(Board::Board* board, bool isAIWhite) {
        std::cout << "Finding optimal move for " << (isAIWhite ? "white" : "black") << std::endl;

        // iterative deeping?
        bool maximizingPlayer = isAIWhite;
        int startingEval = maximizingPlayer ? INITAL_ALPHA : INITAL_BETA;

        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < MAX_ITTERATIVE_DEPTH; i++) {
            std::cout << "Searching at depth " << i << std::endl;
            hasSearchedAtLeastOneMove = false;
            currentItterationDepth = i;

            minimax(board, i, 0, INITAL_ALPHA, INITAL_BETA);

            if (searchCancelled) {
                if (hasSearchedAtLeastOneMove) {
                    bestMove = bestMoveThisItteration;   
                }
                std::cout << "Search aborted at depth " << currentItterationDepth << std::endl;
                break;
            }
            else {
                bestMove = bestMoveThisItteration;
                bestMoveThisItteration = { startingEval, 0, Board::zobrist::Flag::NOTFOUND, {} };
            }
        }

        return bestMove;
    }

    int minimax(Board::Board* board, int playsRemaining, int playsFromRoot, int alpha, int beta) {
        leafCount++;

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

        // stop if we have exceeded the max duration
        if (duration > maxDuration) {
            searchCancelled = true;
            std::cout << "Search cancelled due to time limit. Reached: " << duration << std::endl;
        }
        if(searchCancelled) {
			return 0;
		}

        // Transposition table lookup
        //if (Board::zobrist::transpositionTable.find(board->zobristKey) != Board::zobrist::transpositionTable.end()) {
        //    hitCount++;

        //    Board::zobrist::MoveScore eval = Board::zobrist::transpositionTable.at(board->zobristKey);
        //    
        //    // If the same position has already been searched to at least an equal depth
        //    // to the search we're doing now,we can just use the recorded evaluation.
        //    if (eval.playsFromRoot >= playsFromRoot) {
        //        bestMove = eval;
        //        return eval.evaluation;
        //    }
        //}

        // base case exit condition
        if(playsRemaining == 0) {
			int evaluation = Evaluate::QuiesceEvaluation(*board, alpha, beta);
            return evaluation;
		}

        // minimax algorithm below
        std::vector<Board::Move>& moves = board->whiteTurn ? board->white_moves : board->black_moves;
        std::sort(moves.begin(), moves.end(), &moveSorter);

        int bestValue = -1000000000;

        // if there are no moved we are either in checkmate or stalemate
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

        // for all moves
        for (const Board::Move& move : moves) {
            Board::Board* newBoard = new Board::Board(*board);
            newBoard->move(move, false);

            int score = -minimax(newBoard, playsRemaining - 1, playsFromRoot + 1, -beta, -alpha);

            Board::zobrist::MoveScore moveScore = { score, playsFromRoot + 1, Board::zobrist::EXACT, move };

            Board::zobrist::StoreState(newBoard->zobristKey, moveScore);

            if (score > bestValue) {
                bestValue = score;
                bestMoveThisItteration = moveScore;
                if (score > alpha) {
                    // max in minimax
                    alpha = score;
                }
            }
            if (score >= beta)
                bestMoveThisItteration = moveScore;
                return bestValue; // fail soft beta-cutoff
        }

        return bestValue;
    }

    bool moveSorter(const Board::Move& a, const Board::Move& b) {
        bool positionedBefore = (a.initalPosition.row != b.initalPosition.row) ? a.initalPosition.row < b.initalPosition.row : a.initalPosition.col < b.initalPosition.col;

        int aVal = abs(Board::pieceValues.at(a.piece.piece));
        int bVal = abs(Board::pieceValues.at(b.piece.piece));

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


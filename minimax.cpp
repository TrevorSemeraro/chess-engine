#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <chrono>

#include "pch.h"
#include "Evaluator.cpp"

#define MICROSECONDS_PER_SECOND 1000000
#define MAX_ITTERATIVE_DEPTH 6
#define START_DEPTH 5
#define MATE_SCORE 1000000

/// <summary>
/// Iterative deepening, using a transposition table, embed the depth-first algorithms like alpha-beta into a framework with best-first characteristics.
/// </summary>
namespace MoveGenerator
{

    const int positiveInfinity = 9999999;
    const int negativeInfinity = -9999999;

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
        std::cout << "Current turn on board: " << (board->whiteTurn ? "white" : "black") << std::endl;

        int startingEval = -100000000;
        bestMoveThisItteration = { startingEval, 0, Board::zobrist::Flag::NOTFOUND, {} };

        start = std::chrono::high_resolution_clock::now();

        for (int depth_level = START_DEPTH; depth_level < MAX_ITTERATIVE_DEPTH; depth_level++) {
            std::cout << "Searching at depth " << depth_level << std::endl;
            hasSearchedAtLeastOneMove = false;
            currentItterationDepth = depth_level;

            int playsFromRoot = 0;
            minimax(board, depth_level, playsFromRoot, negativeInfinity, positiveInfinity);

            if (searchCancelled) {
                if (hasSearchedAtLeastOneMove) {
                    bestMove = bestMoveThisItteration;
                }
                std::cout << "Search aborted at depth " << currentItterationDepth << std::endl;
                break;
            }
            else {
                bestMove = bestMoveThisItteration;
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
        if (searchCancelled) {
            return 0;
        }

        // Transposition table lookup
        if (Board::zobrist::transpositionTable.find(board->zobristKey) != Board::zobrist::transpositionTable.end()) {
            hitCount++;

            Board::zobrist::MoveScore eval = Board::zobrist::transpositionTable.at(board->zobristKey);
            
            bool whitePiece = toupper(eval.move.piece) > 0;
            
            if (whitePiece == board->whiteTurn) {

                // If the same position has already been searched to at least an equal depth
                // to the search we're doing now,we can just use the recorded evaluation.
                if (playsFromRoot == 0) {
                    bestMoveThisItteration = eval;
                }

                return eval.evaluation;
            }
        }

        // base case exit condition
        if (playsRemaining == 0) {
            //int evaluation = Evaluate::QuiesceEvaluation(*board, alpha, beta);
            int evaluation = Evaluate::evaluateBoard(*board);
            return evaluation;
        }

        // minimax algorithm below
        std::vector<Board::Move>& moves = board->whiteTurn ? board->white_moves : board->black_moves;
        std::sort(moves.begin(), moves.end(), &moveSorter);

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

        Board::zobrist::Flag evaluationBound = Board::zobrist::UPPERBOUND;
        Board::zobrist::MoveScore bestMoveInPosition = { -1000000, 0, Board::zobrist::Flag::NOTFOUND, {} };

        // for all moves
        for (const Board::Move& move : moves) {
            Board::Board* newBoard = new Board::Board(*board);
            newBoard->move(move, false);

            int score = -minimax(newBoard, playsRemaining - 1, playsFromRoot + 1, -beta, -alpha);

            Board::zobrist::MoveScore moveScore = { score, playsFromRoot + 1, Board::zobrist::EXACT, move };

            if (score >= beta) {
                Board::zobrist::StoreState(board->zobristKey, moveScore, playsRemaining, playsFromRoot, beta, Board::zobrist::Flag::LOWERBOUND);

                // fail soft beta-cutoff
                return beta; 
            }

            if (score > alpha) {
                evaluationBound = Board::zobrist::EXACT;
                bestMoveInPosition = moveScore;

                alpha = score;
                if(playsFromRoot == 0) {
					bestMoveThisItteration = moveScore;
				    hasSearchedAtLeastOneMove = true;
                }
            }
        }

        Board::zobrist::StoreState(board->zobristKey, bestMoveInPosition, playsRemaining, playsFromRoot, beta, evaluationBound);


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


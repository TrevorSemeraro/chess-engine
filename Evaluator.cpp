#include "pch.h"

namespace Evaluate
{
	const int Pawns[64] = {
		0,   0,   0,   0,   0,   0,   0,   0,
		50,  50,  50,  50,  50,  50,  50,  50,
		10,  10,  20,  30,  30,  20,  10,  10,
		5,   5,  10,  25,  25,  10,   5,   5,
		0,   0,   0,  20,  20,   0,   0,   0,
		5,  -5, -10,   0,   0, -10,  -5,   5,
		5,  10,  10, -20, -20,  10,  10,   5,
		0,   0,   0,   0,   0,   0,   0,   0
	};

	const static int PawnsEnd[64] = {
			0,   0,   0,   0,   0,   0,   0,   0,
		80,  80,  80,  80,  80,  80,  80,  80,
		50,  50,  50,  50,  50,  50,  50,  50,
		30,  30,  30,  30,  30,  30,  30,  30,
		20,  20,  20,  20,  20,  20,  20,  20,
		10,  10,  10,  10,  10,  10,  10,  10,
		10,  10,  10,  10,  10,  10,  10,  10,
			0,   0,   0,   0,   0,   0,   0,   0
	};

	const static int Rooks[64] = {
		0,  0,  0,  0,  0,  0,  0,  0,
		5, 10, 10, 10, 10, 10, 10,  5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		0,  0,  0,  5,  5,  0,  0,  0
	};
	const static int Knights[64] = {
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50,
	};
	const static int Bishops[64] = {
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20,
	};
	const static int Queens[64] = {
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		-5,   0,  5,  5,  5,  5,  0, -5,
		0,    0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	};
	const static int KingStart[] =
	{
		-80, -70, -70, -70, -70, -70, -70, -80,
		-60, -60, -60, -60, -60, -60, -60, -60,
		-40, -50, -50, -60, -60, -50, -50, -40,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-20, -30, -30, -40, -40, -30, -30, -20,
		-10, -20, -20, -20, -20, -20, -20, -10,
		20,  20,  -5,  -5,  -5,  -5,  20,  20,
		20,  30,  10,   0,   0,  10,  30,  20
	};

	const static int KingEnd[64] =
	{
		-20, -10, -10, -10, -10, -10, -10, -20,
		-5,   0,   5,   5,   5,   5,   0,  -5,
		-10, -5,   20,  30,  30,  20,  -5, -10,
		-15, -10,  35,  45,  45,  35, -10, -15,
		-20, -15,  30,  40,  40,  30, -15, -20,
		-25, -20,  20,  25,  25,  20, -20, -25,
		-30, -25,   0,   0,   0,   0, -25, -30,
		-50, -30, -30, -30, -30, -30, -30, -50
	};

	int GetValue(char PieceType, Board::Position pos, bool isWhite)
	{
		int row = pos.row;
		int col = pos.col;
		
		PieceType = tolower(PieceType);

		if (!isWhite)
		{
			row = 7 - row;
		}

		int square = Board::PositionToIndex(row, col);

		switch (PieceType) {
		case 'p':
			return Pawns[square];
		case 'r':
			return Rooks[square];
		case 'n':
			return Knights[square];
		case 'b':
			return Bishops[square];
		case 'q':
			return Queens[square];
		case 'k':
			return KingStart[square];
		default:
			return 0;
		}
	}

	int evaluateBoard(Board::Board& board)
	{
		int whiteScore = 0;
		int blackScore = 0;

		int negamaxMultiplier = board.whiteTurn ? 1 : -1;

		if(board.gameState == Board::GameState::Checkmate)
		{
			if(board.whiteWon)
			{
				return 1,000,000;
			}
			else
			{
				return -1,000,000;
			}
		}

		else if(board.gameState == Board::GameState::Draw)
		{
			return 0;
		}

		whiteScore += MaterialScore(board, true);
		whiteScore += PieceSquareScore(board, true);

		blackScore += MaterialScore(board, false);
		blackScore += PieceSquareScore(board, false);

		int score = whiteScore - blackScore;

		return score * negamaxMultiplier;
	}

	int MaterialScore(Board::Board& board, bool white) {
		int score = 0;
		auto pieces = white ? &board.white_pieces : &board.black_pieces;
		for (Board::Piece& p : *pieces)
		{
			const int value = Board::pieceValues.at(p.type);
			score += value;
		}

		return score;
	}

	int PieceSquareScore(Board::Board& board, bool white) {
		int score = 0;
		auto pieces = white ? &board.white_pieces : &board.black_pieces;

		for (Board::Piece& p : *pieces)
		{
			score += GetValue(p.type, p.position, white);
		}

		return score;
	}

	int QuiesceEvaluation(Board::Board& board, int alpha, int beta) {
		int eval = evaluateBoard(board);

		if (eval >= beta)
			return beta;
		if (alpha < eval)
			alpha = eval;

		for(Board::Move& move : board.whiteTurn ? board.white_moves : board.black_moves) {
			if (move.captured != ' ')
			{
				board.move(move, false);
				int score = -QuiesceEvaluation(board, -beta, -alpha);
				board.undoMove(move);

				if (score >= beta)
					return beta;
				if (score > alpha)
					alpha = score;
			}
		}
	
		return alpha;
	}
}
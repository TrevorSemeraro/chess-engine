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

	int GetValue(int piece, Board::Position pos, bool isWhite)
	{
		int row = pos.row;
		int col = pos.col;
		
		if (!isWhite)
		{
			row = 7 - row;
		}

		int square = Board::PositionToIndex(row, col);

		Board::PieceType type = Board::GetPieceType(piece);

		switch (type) {
		case Board::PieceType::Pawn:
			return Pawns[square];
		case Board::PieceType::Rook:
			return Rooks[square];
		case Board::PieceType::Knight:
			return Knights[square];
		case Board::PieceType::Bishop:
			return Bishops[square];
		case Board::PieceType::Queen:
			return Queens[square];
		case Board::PieceType::King:
			return KingStart[square];

		default:
			return 0;
		}
	}

	int evaluateBoard(Board::Board& board)
	{
		int score = 0;

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
			return -100;
		}

		score += MaterialScore(board);
		score += PieceSquareScore(board);

		int negamaxMultiplier = board.whiteTurn ? 1 : -1;

		return score;
	}

	int MaterialScore(Board::Board& board) {
		int score = 0;

		for (Board::Piece& p : board.white_pieces)
		{
			const int value = Board::pieceValues.at(p.piece);
			score += value;
		}

		for (Board::Piece& p : board.black_pieces)
		{
			const int value = Board::pieceValues.at(p.piece);
			score += value;
		}

		return score;
	}

	int PieceSquareScore(Board::Board& board) {
		int score = 0;

		for (Board::Piece& p : board.white_pieces)
		{
			score += GetValue(p.piece, p.position, true);
		}

		for (Board::Piece& p : board.black_pieces)
		{
			score -= GetValue(p.piece, p.position, false);
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
			if (move.captured != ' ' && move.piece.piece > 0)
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
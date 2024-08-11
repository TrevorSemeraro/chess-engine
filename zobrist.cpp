#include <unordered_map>
#include <random>

#include "pch.h"

namespace Board
{
	namespace zobrist
	{

		std::unordered_map<uint64_t, MoveScore> transpositionTable = {};
		uint64_t whiteToMoveKey;
		uint64_t castlingKeys[16];
		uint64_t pieceKeys[2][6][64];
		uint64_t enPassantKeys[8];
		uint64_t blackToMove;

		void init() {
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::mt19937_64 gen(seed);
			std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);

			blackToMove = dis(gen);

			for (int i = 0; i < 16; i++)
			{
				castlingKeys[i] = dis(gen);
			}

			for (int i = 0; i < 8; i++)
			{
				enPassantKeys[i] = dis(gen);
			}

			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 6; j++)
				{
					for (int k = 0; k < 64; k++)
					{
						pieceKeys[i][j][k] = dis(gen);
					}
				}
			}
		}

		int castlingStateToInt(Board& board, bool white) {
			int state = 0;

			if (white) {
				if (board.canWhiteCastleKing && board.canWhiteCastleQueen)
					state = 1;
				else if (board.canWhiteCastleQueen)
					state = 2;
				else if (board.canWhiteCastleKing)
					state = 3;
				else
					state = 0;
			}
			else {
				if(board.canBlackCastleKing && board.canBlackCastleQueen)
					state = 1;
				else if (board.canBlackCastleQueen)
					state = 2;
				else if (board.canBlackCastleKing)
					state = 3;
				else
					state = 0;
			}

			return state;
		}

		uint64_t generateHash(Board& b) {
			uint64_t key = 0;

			if (!b.whiteTurn)
				key ^= blackToMove;

			// turn castling state into number 0-15
			// white has 4 options, black has 4 options
			// none, both, king, queen => 0, 1, 2, 3
			
			// each have a corresponding 4 options
			// 00, 01, 02, 03, 10, 11, 12, 13, 20, 21, 22, 23, 30, 31, 32, 33
			
			// white state * 4 + black
			int whiteCastleState = castlingStateToInt(b, true);
			int blackCastleState = castlingStateToInt(b, false);

			int castlingKey = whiteCastleState * 4 + blackCastleState;

			key ^= castlingKeys[castlingKey];

			for (int i = 0; i < 64; i++)
			{
				if (b.board[i] != ' ')
				{
					int pieceValue = b.board[i];
					
					bool isWhite = GetPieceColour(pieceValue);
					int color = isWhite ? 1 : 0;

					int pieceIndex = GetPieceType(pieceValue);

					key ^= pieceKeys[color][pieceIndex][i];
				}
			}

			return key;
		}

		uint64_t updateHash(Board& board, Move& move) {
			// for the last move, xor out the last position
			int initalposition = PositionToIndex(move.initalPosition.row, move.initalPosition.col);
			int finalposition = PositionToIndex(move.finalPosition.row, move.finalPosition.col);

			Piece piece = move.piece;
			PieceType pieceType = GetPieceType(piece.piece);
			
			bool isWhitePiece = move.piece.white;

			uint64_t inital = board.zobristKey;

			//this->board[initalposition] = ' ';
			int color = isWhitePiece ? 1 : 0;
			inital ^= pieceKeys[color][pieceType][initalposition]; // xor out the old position

			// xor in the new position
			inital ^= pieceKeys[color][pieceType][finalposition];

			// xor out any captures
			if (move.willCapture) {
				color = isupper(move.captured) > 0 ? 1 : 0;
				int capturedPieceIndex = GetPieceType(move.captured);
				inital ^= pieceKeys[color][capturedPieceIndex][finalposition];
			}

			return inital;
		}

		void StoreState(uint64_t state, MoveScore score)
		{
			transpositionTable[state] = score;
		}

		MoveScore GetState(uint64_t state, int playsRemaining, int playsFromRoot, int alpha, int beta)
		{
			return transpositionTable.at(state);
		}
	}
}
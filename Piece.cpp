#include <vector>
#include <unordered_map>

#include "pch.h"

namespace Board
{
	const std::unordered_map<int, int> pieceValues = {
		{WhitePawn, PAWN_VALUE},
		{WhiteRook, ROOK_VALUE},
		{WhiteKnight, KNIGHT_VALUE},
		{WhiteBishop, BISHOP_VALUE},
		{WhiteQueen, QUEEN_VALUE},
		{WhiteKing, KING_VALUE},

		{BlackPawn, -PAWN_VALUE},
		{BlackRook, -ROOK_VALUE},
		{BlackKnight, -KNIGHT_VALUE},
		{BlackBishop, -BISHOP_VALUE},
		{BlackQueen, -QUEEN_VALUE},
		{BlackKing, -KING_VALUE},

		{0, 0}
	};

	Piece::Piece() {
		Piece({0, 0}, PieceType::None, PieceColour::White);
	}

	Piece::Piece(Position pos, PieceType type, PieceColour white) {
		this->position = pos;
		this->piece = type | white;
		this->white = white == PieceColour::White;
	}

	int CreatePiece(int pieceType, int pieceColour) {
		return pieceType | pieceColour;
	}

	int CreatePiece(int pieceType, bool isWhite) {
		return CreatePiece(pieceType, isWhite ? PieceColour::White : PieceColour::Black);
	}

	PieceColour GetPieceColour(int piece) {
		return static_cast<PieceColour>(piece & colourMask);
	}

	PieceType GetPieceType(int piece) {
		return static_cast<PieceType>(piece & typeMask);
	}

	PieceType charToPieceType(char c) {
		switch (tolower(c)) {
		case 'p':
			return PieceType::Pawn;
		case 'r':
			return PieceType::Rook;
		case 'n':
			return PieceType::Knight;
		case 'b':
			return PieceType::Bishop;
		case 'q':
			return PieceType::Queen;
		case 'k':
			return PieceType::King;
		default:
			return PieceType::None;
		}
	}
}

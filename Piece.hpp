#ifndef PIECE_HPP
#define PIECE_HPP

#include <stdexcept>

enum Piece {
	NONE = -1,
	PAWN = 0,
	KNIGHT = 1,
	BISHOP = 2,
	ROOK = 3,
	QUEEN = 4,
	KING = 5,
};
enum ColoredPiece {
	COLORED_NONE = -1,
	WHITE_PAWN = 0,
	WHITE_KNIGHT = 1,
	WHITE_BISHOP = 2,
	WHITE_ROOK = 3,
	WHITE_QUEEN = 4,
	WHITE_KING = 5,
	BLACK_PAWN = 6,
	BLACK_KNIGHT = 7,
	BLACK_BISHOP = 8,
	BLACK_ROOK = 9,
	BLACK_QUEEN = 10,
	BLACK_KING = 11,
};

struct PieceHelper {
	static inline ColoredPiece getPiece(char piece) {
		switch (piece) {
			case 'P': return ColoredPiece::WHITE_PAWN;
			case 'N': return ColoredPiece::WHITE_KNIGHT;
			case 'B': return ColoredPiece::WHITE_BISHOP;
			case 'R': return ColoredPiece::WHITE_ROOK;
			case 'Q': return ColoredPiece::WHITE_QUEEN;
			case 'K': return ColoredPiece::WHITE_KING;
			case 'p': return ColoredPiece::BLACK_PAWN;
			case 'n': return ColoredPiece::BLACK_KNIGHT;
			case 'b': return ColoredPiece::BLACK_BISHOP;
			case 'r': return ColoredPiece::BLACK_ROOK;
			case 'q': return ColoredPiece::BLACK_QUEEN;
			case 'k': return ColoredPiece::BLACK_KING;
		default:
			throw std::invalid_argument("Invalid FEN piece character");
		}
	}
	static inline char getChar(ColoredPiece piece) {
		switch (piece) {
			case ColoredPiece::WHITE_PAWN: return 'P';
			case ColoredPiece::WHITE_KNIGHT: return 'N';
			case ColoredPiece::WHITE_BISHOP: return 'B';
			case ColoredPiece::WHITE_ROOK: return 'R';
			case ColoredPiece::WHITE_QUEEN: return 'Q';
			case ColoredPiece::WHITE_KING: return 'K';
			case ColoredPiece::BLACK_PAWN: return 'p';
			case ColoredPiece::BLACK_KNIGHT: return 'n';
			case ColoredPiece::BLACK_BISHOP: return 'b';
			case ColoredPiece::BLACK_ROOK: return 'r';
			case ColoredPiece::BLACK_QUEEN: return 'q';
			case ColoredPiece::BLACK_KING: return 'k';
		}

		return '_';
	}
	static inline std::string getName(Piece piece) {
		switch (piece) {
			case Piece::PAWN: return "Pawn";
			case Piece::KNIGHT: return "Knight";
			case Piece::BISHOP: return "Bishop";
			case Piece::ROOK: return "Rook";
			case Piece::QUEEN: return "Queen";
			case Piece::KING: return "King";
			default: return "None";
		}
	}

	static inline Piece getPieceType(ColoredPiece piece) {
		if (piece == ColoredPiece::COLORED_NONE) return Piece::NONE;
		return static_cast<Piece>(piece % 6);
	}
	static inline ColoredPiece getColoredPieceType(Piece piece, bool white) {
		if (piece == Piece::NONE) return ColoredPiece::COLORED_NONE;
		return static_cast<ColoredPiece>(piece + (6 * (!white)));
	}

	PieceHelper() = delete;
	~PieceHelper() = delete;
};

#endif
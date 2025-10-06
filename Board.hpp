#ifndef BOARD_HPP
#define BOARD_HPP

#include "Bitboard.hpp"
#include <array>

struct BoardInfo {
	// rook starting positions.
	static constexpr bit whiteLeftRook = 0b1ULL;
	static constexpr bit whiteRightRook = 0b10000000ULL;
	static constexpr bit blackLeftRook = 0b1ULL << 56;
	static constexpr bit blackRightRook = 0b10000000ULL << 56;
	
	// king starting positions.
	static constexpr bit whiteKing = 0b10000ULL;
	static constexpr bit blackKing = 0b10000ULL << 56;

	// king casling positions.
	static constexpr bit whiteKingLeftCastle = 0b100ULL;
	static constexpr bit whiteKingRightCastle = 0b1000000ULL;
	static constexpr bit blackKingLeftCastle = 0b100ULL << 56;
	static constexpr bit blackKingRightCastle = 0b1000000ULL << 56;

	// king castling attacks.
	static constexpr bitboard whiteKingLeftCasleAttacks = 0b1100ULL;
	static constexpr bitboard whiteKingRightCastleAttacks = 0b1100000ULL;
	static constexpr bitboard blackKingLeftCastleAttacks = 0b1100ULL << 56;
	static constexpr bitboard blackKingRightCastleAttacks = 0b1100000ULL << 56;

public:
	const bool whiteMove;

	const bool enPassant;

	// castling rights.
	const bool whiteLeftCastle;
	const bool whiteRightCastle;
	const bool blackLeftCastle;
	const bool blackRightCastle;

	constexpr BoardInfo(bool whiteMove, bool enPassant, bool whiteLeftCastle, 
		bool whiteRightCastle, bool blackLeftCastle, bool blackRightCastle) :
		whiteMove(whiteMove), enPassant(enPassant), whiteLeftCastle(whiteLeftCastle),
		whiteRightCastle(whiteRightCastle), blackLeftCastle(blackLeftCastle), blackRightCastle(blackRightCastle)
	{}

	constexpr inline bool canCastle() const {
		if (this->whiteMove) {
			return this->whiteLeftCastle || this->whiteRightCastle;
		}
		else {
			return this->blackLeftCastle || this->blackRightCastle;
		}
	}

	constexpr inline bool canCastleLeft() const {
		if (this->whiteMove) {
			return this->whiteLeftCastle;
		}
		else {
			return this->blackLeftCastle;
		}
	}

	constexpr inline bool canCastleRight() const {
		if (this->whiteMove) {
			return this->whiteRightCastle;
		}
		else {
			return this->blackRightCastle;
		}
	}

	constexpr BoardInfo kingMoved() {
		if (this->whiteMove) {
			return BoardInfo(false, false, false, false, this->blackLeftCastle, this->blackRightCastle);
		}
		else {
			return BoardInfo(true, false, this->whiteLeftCastle, this->whiteRightCastle, false, false);
		}
	}

	constexpr BoardInfo pawnMovedTwoSquares() {
		return BoardInfo(!this->whiteMove, true, this->whiteLeftCastle, 
			this->whiteRightCastle, this->blackLeftCastle, this->blackRightCastle);
	}

	constexpr BoardInfo leftRookMoved() {
		if (this->whiteMove) {
			return BoardInfo(false, false, false, this->whiteRightCastle, this->blackLeftCastle, this->blackRightCastle);
		}
		else {
			return BoardInfo(true, false, this->whiteLeftCastle, this->whiteRightCastle, false, this->blackRightCastle);
		}
	}

	constexpr BoardInfo rightRookMoved() {
		if (this->whiteMove) {
			return BoardInfo(false, false, this->whiteLeftCastle, false, this->blackLeftCastle, this->blackRightCastle);
		}
		else {
			return BoardInfo(true, false, this->whiteLeftCastle, this->whiteRightCastle, this->blackLeftCastle, false);
		}
	}
};

class Board {
private:
	enum : uint8_t {
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

	std::array<bitboard, 12> pieces;

	BoardInfo info;


};

#endif
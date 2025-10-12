#ifndef BOARD_INFO_HPP
#define BOARD_INFO_HPP

#include "Bitboard.hpp"

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
	bool whiteMove;

	Index enPassantTarget;

	// castling rights.
	bool whiteLeftCastle;
	bool whiteRightCastle;
	bool blackLeftCastle;
	bool blackRightCastle;

	// 
	uint16_t halfmoves;
	uint16_t fullmoves;

	char endState;

	bitboard checkingPieces;
	bitboard rookPins;
	bitboard bishopPins;

	BoardInfo() : whiteMove(true), enPassantTarget(-1), whiteLeftCastle(true),
		whiteRightCastle(true), blackLeftCastle(true), blackRightCastle(true), halfmoves(0), fullmoves(1), endState('p'), 
		checkingPieces(0ULL), rookPins(0ULL), bishopPins(0ULL)
	{
	}

	inline bool canCastle() const {
		if (this->whiteMove) {
			return this->whiteLeftCastle || this->whiteRightCastle;
		}
		else {
			return this->blackLeftCastle || this->blackRightCastle;
		}
	}

	inline bool canCastleLeft() const {
		if (this->whiteMove) {
			return this->whiteLeftCastle;
		}
		else {
			return this->blackLeftCastle;
		}
	}

	inline bool canCastleRight() const {
		if (this->whiteMove) {
			return this->whiteRightCastle;
		}
		else {
			return this->blackRightCastle;
		}
	}

	void kingMoved() {
		if (this->whiteMove) {
			this->whiteLeftCastle = false;
			this->whiteRightCastle = false;
		}
		else {
			this->blackLeftCastle = false;
			this->blackRightCastle = false;
		}
	}

	void pawnMovedTwoSquares(Index enPassantTarget) {
		this->enPassantTarget = enPassantTarget;
	}
	void pawnDidntMoveTwoSquares() {
		this->enPassantTarget = -1;
	}

	void leftRookMoved() {
		if (this->whiteMove) {
			this->whiteLeftCastle = false;
		}
		else {
			this->blackLeftCastle = false;
		}
	}
	void rightRookMoved() {
		if (this->whiteMove) {
			this->whiteRightCastle = false;
		}
		else {

			this->blackRightCastle = false;
		}
	}
	void toggleTurn() { 
		this->whiteMove = !this->whiteMove;
		if (this->whiteMove) this->fullmoves++; 
	}
};

#endif
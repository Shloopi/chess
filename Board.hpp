#ifndef BOARD_HPP
#define BOARD_HPP

#include <array>
#include <iostream>
#include "Zobrist.hpp"
#include "Fen.hpp"
#include <unordered_map>
#include <stack>
#include "BoardInfo.hpp"
#include <functional>
#include "Constants.hpp"

class Board {
private:
	static constexpr uint8_t leftCastleMask = 0b1010;
	static constexpr uint8_t rightCastleMask = 0b0101;
	static constexpr uint8_t whiteCastleMask = 0b1100;
	static constexpr uint8_t blackCastleMask = 0b0011;

public:
	bitboard whitePawns{ 0 }, whiteKnights{ 0 }, whiteBishops{ 0 }, whiteRooks{ 0 }, whiteQueens{ 0 };
	bitboard blackPawns{ 0 }, blackKnights{ 0 }, blackBishops{ 0 }, blackRooks{ 0 }, blackQueens{ 0 };
	Index whiteKing{ 0 }, blackKing{ 0 };

	uint8_t castlingRights {0b1111}; // white left, white right, black left, black right
	Index enPassant { 0 };
	Board() = default;

	Board(bitboard wp, bitboard wn, bitboard wb, bitboard wr, bitboard wq, Index wk,
		bitboard bp, bitboard bn, bitboard bb, bitboard br, bitboard bq, Index bk, uint8_t castling, Index ep) :
		whitePawns(wp), whiteKnights(wn), whiteBishops(wb), whiteRooks(wr), whiteQueens(wq), whiteKing(wk),
		blackPawns(bp), blackKnights(bn), blackBishops(bb), blackRooks(br), blackQueens(bq), blackKing(bk), castlingRights(castling), enPassant(ep)
	{}

	template <bool whiteToMove>
	constexpr inline bitboard getPawns() {
		if constexpr (whiteToMove) {
			return this->whitePawns;
		}
		else {
			return this->blackPawns;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getKnights() {
		if constexpr (whiteToMove) {
			return this->whiteKnights;
		}
		else {
			return this->blackKnights;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getBishops() {
		if constexpr (whiteToMove) {
			return this->whiteBishops;
		}
		else {
			return this->blackBishops;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getRooks() {
		if constexpr (whiteToMove) {
			return this->whiteRooks;
		}
		else {
			return this->blackRooks;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getQueens() {
		if constexpr (whiteToMove) {
			return this->whiteQueens;
		}
		else {
			return this->blackQueens;
		}
	}

	template <bool whiteToMove>
	constexpr inline Index getKing() {
		if constexpr (whiteToMove) {
			return this->whiteKing;
		}
		else {
			return this->blackKing;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getAllPieces() {
		if constexpr (whiteToMove) {
			return this->whitePawns | this->whiteKnights | this->whiteBishops | this->whiteRooks | this->whiteQueens | Constants::SQUARE_BBS[this->whiteKing];
		}
		else {
			return this->blackPawns | this->blackKnights | this->blackBishops | this->blackRooks | this->blackQueens | Constants::SQUARE_BBS[this->blackKing];
		}
	}

	constexpr inline bitboard getAllPieces() {
		return this->getAllPieces<true>() | this->getAllPieces<false>();
	}

	constexpr inline bitboard getFreeSquares() {
		return ~this->getAllPieces();
	}

	template <Piece piece, bool whiteToMove>
	constexpr inline void setPiece(Index index) {
		if constexpr (piece == Chess::KING) {
			if constexpr (whiteToMove) this->whiteKing = index;
			else this->blackKing = index;
		}
		else {
			Chess::setBit(this->getPiece<piece, whiteToMove>(), index);
		}
	}

	template <bool whiteToMove>
	inline void setPiece(Piece piece, Index index) {
		switch (piece) {
		case Chess::PAWN:
			this->setPiece<Chess::PAWN, whiteToMove>(index);
			break;
		case Chess::KNIGHT:
			this->setPiece<Chess::KNIGHT, whiteToMove>(index);
			break;
		case Chess::BISHOP:
			this->setPiece<Chess::BISHOP, whiteToMove>(index);
			break;
		case Chess::ROOK:
			this->setPiece<Chess::ROOK, whiteToMove>(index);
			break;
		case Chess::QUEEN:
			this->setPiece<Chess::QUEEN, whiteToMove>(index);
			break;
		case Chess::KING:
			this->setPiece<Chess::KING, whiteToMove>(index);
			break;
		default:
			throw std::invalid_argument("Board::setPiece - invalid piece type");
		}
	}

	inline void setCastlingRights(bool whiteLeft, bool whiteRight, bool blackLeft, bool blackRight) {
		this->castlingRights = (whiteLeft ? 0b1000 : 0) | (whiteRight ? 0b0100 : 0) | (blackLeft ? 0b0010 : 0) | (blackRight ? 0b0001 : 0);
	}

	template <bool whiteToMove>
	constexpr inline bool canCastleLeft() const {
		if constexpr (whiteToMove) {
			return (this->castlingRights & leftCastleMask & whiteCastleMask) != 0;
		}
		else {
			return (this->castlingRights & leftCastleMask & blackCastleMask) != 0;
		}
	}

	template <bool whiteToMove>
	constexpr inline bool canCastleRight() const {
		if constexpr (whiteToMove) {
			return (this->castlingRights & rightCastleMask & whiteCastleMask) != 0;
		}
		else {
			return (this->castlingRights & rightCastleMask & blackCastleMask) != 0;
		}
	}
	
	template <bool whiteToMove>
	constexpr inline bool canCastle() const {
		if constexpr (whiteToMove) {
			return (this->castlingRights & whiteCastleMask) != 0;
		}
		else {
			return (this->castlingRights & blackCastleMask) != 0;
		}
	}
};

#endif
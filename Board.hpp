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
	static constexpr uint8_t whiteShortCastleMask = 0b100;
	static constexpr uint8_t whiteLongCastleMask = 0b1000;
	static constexpr uint8_t blackShortCastleMask = 0b1;
	static constexpr uint8_t blackLongCastleMask = 0b10;

	static constexpr uint8_t longCastleMask = whiteLongCastleMask | blackLongCastleMask;
	static constexpr uint8_t shortCastleMask = whiteShortCastleMask | blackShortCastleMask;
	static constexpr uint8_t whiteCastleMask = whiteShortCastleMask | whiteLongCastleMask;
	static constexpr uint8_t blackCastleMask = blackShortCastleMask | blackLongCastleMask;

	static constexpr bitboard whiteQueensideRook = 0b1;
	static constexpr bitboard whiteKingsideRook = 0b10000000;
	static constexpr bitboard blackQueensideRook = whiteQueensideRook << 56;
	static constexpr bitboard blackKingsideRook = whiteQueensideRook << 56;

	static constexpr bitboard whiteRookShortCastle = 0b100000;
	static constexpr bitboard whiteRookLongCastle = 0b1000;
	static constexpr bitboard blackRookShortCastle = whiteRookShortCastle << 56;
	static constexpr bitboard blackRookLongCastle = whiteRookLongCastle << 56;



public:
	bitboard whitePawns{ 0 }, whiteKnights{ 0 }, whiteBishops{ 0 }, whiteRooks{ 0 }, whiteQueens{ 0 };
	bitboard blackPawns{ 0 }, blackKnights{ 0 }, blackBishops{ 0 }, blackRooks{ 0 }, blackQueens{ 0 };
	Index whiteKing{ 0 }, blackKing{ 0 };

	uint8_t castlingRights {0b1111}; // white left, white right, black left, black right
	bitboard enPassant { 0 };
	Board() = default;

	Board(bitboard wp, bitboard wn, bitboard wb, bitboard wr, bitboard wq, Index wk,
		bitboard bp, bitboard bn, bitboard bb, bitboard br, bitboard bq, Index bk, uint8_t castling, bitboard ep) :
		whitePawns(wp), whiteKnights(wn), whiteBishops(wb), whiteRooks(wr), whiteQueens(wq), whiteKing(wk),
		blackPawns(bp), blackKnights(bn), blackBishops(bb), blackRooks(br), blackQueens(bq), blackKing(bk), castlingRights(castling), enPassant(ep)
	{}

	template <bool whiteToMove>
	constexpr inline bitboard startingRooks() {
		return this->getLeftRookStartingPos<whiteToMove>() | this->getRightRookStartingPos<whiteToMove>();
	}

	template <bool whiteToMove>
	constexpr inline bitboard startingQueensideRook() {
		if constexpr (whiteToMove) {
			return Board::whiteQueensideRook;
		}
		else {
			return Board::blackQueensideRook;
		}
	}

	template <bool whiteToMove> 
	constexpr inline bitboard startingKingsideRook() {
		if constexpr (whiteToMove) {
			return Board::whiteKingsideRook;
		}
		else {
			return Board::blackKingsideRook;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getRookShortCastlingMove() {
		if constexpr (whiteToMove) {
			return Board::whiteKingsideRook | Board::whiteRookShortCastle;
		}
		else {
			return Board::blackKingsideRook | Board::blackRookShortCastle;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getRookLongCastlingMove() {
		if constexpr (whiteToMove) {
			return Board::whiteQueensideRook | Board::whiteRookLongCastle;
		}
		else {
			return Board::blackQueensideRook | Board::blackRookLongCastle;
		}
	}

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
	constexpr inline bool canCastleShort() const {
		if constexpr (whiteToMove) {
			return (this->castlingRights & shortCastleMask & whiteCastleMask) != 0;
		}
		else {
			return (this->castlingRights & shortCastleMask & blackCastleMask) != 0;
		}
	}

	template <bool whiteToMove>
	constexpr inline bool canCastleLong() const {
		if constexpr (whiteToMove) {
			return (this->castlingRights & longCastleMask & whiteCastleMask) != 0;
		}
		else {
			return (this->castlingRights & longCastleMask & blackCastleMask) != 0;
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

	template <bool whiteToMove, Piece piece, Flag flag = Chess::QUIET>
	void makeMove(bitboard from, bitboard to) {
		bitboard change = from | to;
		bitboard notFrom = ~from;
		bitboard notTo = ~to;
		bitboard enPassantTemp = this->enPassant;

		// handle pawn moved 2 squares.
		if constexpr (flag == Chess::DOUBLE_PAWN_PUSH) {
			this->enPassant = Chess::pawnForward<whiteToMove>(from);
		}
		else {
			this->enPassant = 0;
		}

		// check promotions.
		if constexpr (flag == Chess::BISHOP_PROMOTION || flag == Chess::ROOK_PROMOTION || flag == Chess::KNIGHT_PROMOTION || flag == Chess::QUEEN_PROMOTION) {
			if constexpr (whiteToMove) {
				this->blackPawns &= notTo;
				this->blackKnights &= notTo;
				this->blackRooks &= notTo;
				this->blackQueens &= notTo;

				this->whitePawns &= notFrom;
			}
			else {
				this->whitePawns &= notTo;
				this->whiteKnights &= notTo;
				this->whiteRooks &= notTo;
				this->whiteQueens &= notTo;

				this->blackPawns &= notFrom;
			}

			if constexpr (flag == Chess::BISHOP_PROMOTION) {
				if constexpr (whiteToMove) {
					this->whiteBishops |= to;
				}
				else {
					this->blackBishops |= to;
				}
			}
			else if constexpr (flag == Chess::ROOK_PROMOTION) {
				if constexpr (whiteToMove) {
					this->whiteRooks |= to;
				}
				else {
					this->blackRooks |= to;
				}
			}
			else if constexpr (flag == Chess::QUEEN_PROMOTION) {
				if constexpr (whiteToMove) {
					this->whiteQueens |= to;
				}
				else {
					this->blackQueens |= to;
				}
			}
			else if constexpr (flag == Chess::KNIGHT_PROMOTION) {
				if constexpr (whiteToMove) {
					this->whiteKnights |= to;
				}
				else {
					this->blackKnights |= to;
				}
			}

			return;
		}

		// handle castling rights.
		if constexpr (flag == Chess::REMOVE_ALL_CASTLING || flag == Chess::SHORT_CASTLING || flag == Chess::LONG_CASTLING) {
			if constexpr (whiteToMove) {
				this->castlingRights &= ~whiteCastleMask;
			}
			else {
				this->castlingRights &= ~blackCastleMask;
			}
		}
		else if constexpr (flag == Chess::REMOVE_SHORT_CASTLING) {
			if constexpr (whiteToMove) {
				this->castlingRights &= ~(whiteCastleMask & shortCastleMask);
			}
			else {
				this->castlingRights &= ~(blackCastleMask & shortCastleMask);
			}
		}
		else if constexpr (flag == Chess::REMOVE_LONG_CASTLING) {
			if constexpr (whiteToMove) {
				this->castlingRights &= ~(whiteCastleMask & longCastleMask);
			}
			else {
				this->castlingRights &= ~(blackCastleMask & longCastleMask);
			}
		}

		// checking if capturing enemy rooks affect enemy castling rights.
		bitboard capture = to & Board::startingRooks<!whiteToMove>();
		if (capture == Board::startingQueensideRook<!whiteToMove>()) {
				if constexpr (whiteToMove) {
					this->castlingRights &= ~whiteLongCastleMask;
				}
				else {
					this->castlingRights &= ~blackLongCastleMask;
				}
			}
		else if (capture == Board::startingKingsideRook<!whiteToMove>()) {
			if constexpr (whiteToMove) {
				this->castlingRights &= ~whiteShortCastleMask;
			}
			else {
				this->castlingRights &= ~blackShortCastleMask;
			}
		}

		// handle castle.
		if constexpr (flag == Chess::SHORT_CASTLING) {
			if constexpr (whiteToMove) {
				this->whiteRooks ^= this->getRookShortCastlingMove<whiteToMove>();
				this->whiteKing = Chess::lsb(to);
			}
			else {
				this->blackRooks ^= this->getRookShortCastlingMove<whiteToMove>();
				this->blackKing = Chess::lsb(to);
			}

			return;
		}
		else if constexpr (flag == Chess::LONG_CASTLING) {
			if constexpr (whiteToMove) {
				this->whiteRooks ^= this->getRookLongCastlingMove<whiteToMove>();
				this->whiteKing = Chess::lsb(to);
			}
			else {
				this->blackRooks ^= this->getRookLongCastlingMove<whiteToMove>();
				this->blackKing = Chess::lsb(to);
			}

			return;
		}

		// remove possible captures.
		if constexpr (whiteToMove) {
			this->blackPawns &= notTo;
			this->blackKnights &= notTo;
			this->blackBishops &= notTo;
			this->blackRooks &= notTo;
			this->blackQueens &= notTo;
		}
		else {
			this->whitePawns &= notTo;
			this->whiteKnights &= notTo;
			this->whiteBishops &= notTo;
			this->whiteRooks &= notTo;
			this->whiteQueens &= notTo;
		}

		// handle normal move.
		if constexpr (piece == Chess::PAWN) {
			bitboard enPassantMask = Chess::MAX_BITBOARD;

			if constexpr (flag == Chess::EN_PASSANT) enPassantMask &= ~enPassantTemp;

			if constexpr (whiteToMove) {
				this->whitePawns ^= change;
				this->blackPawns &= enPassantMask;
			}
			else {
				this->blackPawns ^= change;
				this->whitePawns &= enPassantMask;
			}
		}
		else if constexpr (piece == Chess::KNIGHT) {
			if constexpr (whiteToMove) {
				this->whiteKnights ^= change;
			}
			else {
				this->blackKnights ^= change;
			}
		}
		else if constexpr (piece == Chess::BISHOP) {
			if constexpr (whiteToMove) {
				this->whiteBishops ^= change;
			}
			else {
				this->blackBishops ^= change;
			}
		}
		else if constexpr (piece == Chess::ROOKS) {
			if constexpr (whiteToMove) {
				this->whiteRooks ^= change;
			}
			else {
				this->blackRooks ^= change;
			}
		}
		else if constexpr (piece == Chess::QUEEN) {
			if constexpr (whiteToMove) {
				this->whiteQueens ^= change;
			}
			else {
				this->blackQueens ^= change;
			}
		}
		else if constexpr (piece == Chess::KING) {
			if constexpr (whiteToMove) {
				this->whiteKing = Chess::lsb(to);
			}
			else {
				this->blackKing = Chess::lsb(to);
			}
		}
	}

	template <bool whiteToMove>
	void makeMove(Move move) {
		switch (move.piece) {
			case Chess::PAWN:
			{
				switch (move.flag) {
					case Chess::QUIET:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::QUIET>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::DOUBLE_PAWN_PUSH:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::DOUBLE_PAWN_PUSH>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::EN_PASSANT:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::EN_PASSANT>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::KNIGHT_PROMOTION:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::KNIGHT_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::BISHOP_PROMOTION:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::BISHOP_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::ROOK_PROMOTION:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::ROOK_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::QUEEN_PROMOTION:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::QUEEN_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
				}
			}
			case Chess::KNIGHT:
			{
				this->makeMove<whiteToMove, Chess::KNIGHT>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
				return;
			}
			case Chess::BISHOP:
			{
				this->makeMove<whiteToMove, Chess::BISHOP>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
				return;
			}
			case Chess::ROOK:
			{
				switch (move.flag) {
					case Chess::REMOVE_SHORT_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::ROOK, Chess::REMOVE_SHORT_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::REMOVE_LONG_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::ROOK, Chess::REMOVE_LONG_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
				}
			}
			case Chess::QUEEN:
			{
				this->makeMove<whiteToMove, Chess::QUEEN>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
				return;
			}
			case Chess::KING:
			{
				switch (move.flag) {
					case Chess::REMOVE_ALL_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::KING, Chess::REMOVE_ALL_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::SHORT_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::KING, Chess::SHORT_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::LONG_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::KING, Chess::LONG_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
				}
			}
		}
	}

	void makeMove(Move move, bool whiteToMove) {
		if (whiteToMove) {
			this->makeMove<true>(move);
		}
		else {
			this->makeMove<false>(move);
		}
	}
};

#endif
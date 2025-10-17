#ifndef CHESS_HPP
#define CHESS_HPP

#include <cstdint>
#include <bit>
#include <iostream>

using bitboard = uint64_t;
using File = int8_t;
using Rank = int8_t;
using Index = int8_t;
using Piece = uint8_t;
using Flag = uint8_t;

namespace Chess {
	constexpr uint64_t BOARD_SIZE = 64;
	constexpr uint64_t RANK_SIZE = 8;
	constexpr uint64_t FILE_SIZE = 8;

	// ------------- Pieces -------------
	constexpr Piece PAWN = 0;
	constexpr Piece KNIGHT = 1;
	constexpr Piece BISHOP = 2;
	constexpr Piece ROOK = 3;
	constexpr Piece QUEEN = 4;
	constexpr Piece KING = 5;

	// ------------- Flags -------------
	constexpr Flag QUIET = 0;
	constexpr Flag REMOVE_SHORT_CASTLING = 1;
	constexpr Flag REMOVE_LONG_CASTLING = 2;
	constexpr Flag REMOVE_ALL_CASTLING = 3;
	constexpr Flag DOUBLE_PAWN_PUSH = 4;
	constexpr Flag EN_PASSANT = 5;
	constexpr Flag KNIGHT_PROMOTION = 6;
	constexpr Flag BISHOP_PROMOTION = 7;
	constexpr Flag ROOK_PROMOTION = 8;
	constexpr Flag QUEEN_PROMOTION = 9;
	constexpr Flag SHORT_CASTLING = 10;
	constexpr Flag LONG_CASTLING = 11;

	// ------------- GEOMETRY -------------
	constexpr bitboard RANK1 = 0x00000000000000FFULL;
	constexpr bitboard RANK2 = 0x000000000000FF00ULL;
	constexpr bitboard RANK3 = 0x0000000000FF0000ULL;
	constexpr bitboard RANK4 = 0x00000000FF000000ULL;
	constexpr bitboard RANK5 = 0x000000FF00000000ULL;
	constexpr bitboard RANK6 = 0x0000FF0000000000ULL;
	constexpr bitboard RANK7 = 0x00FF000000000000ULL;
	constexpr bitboard RANK8 = 0xFF00000000000000ULL;

	constexpr bitboard FILE_A = 0x0101010101010101ULL;
	constexpr bitboard FILE_B = 0x0202020202020202ULL;
	constexpr bitboard FILE_C = 0x0404040404040404ULL;
	constexpr bitboard FILE_D = 0x0808080808080808ULL;
	constexpr bitboard FILE_E = 0x1010101010101010ULL;
	constexpr bitboard FILE_F = 0x2020202020202020ULL;
	constexpr bitboard FILE_G = 0x4040404040404040ULL;
	constexpr bitboard FILE_H = 0x8080808080808080ULL;

	constexpr bitboard DIAG = 0x8040201008040201ULL;
	constexpr bitboard OPP_DIAG = 0x0102040810204080ULL;

	// ------------- Bitboards -------------
	constexpr bitboard MAX_BITBOARD = 0xFFFFFFFFFFFFFFFFULL;

	static constexpr Index rankOf(Index square) {
		return square / RANK_SIZE;
	}

	static constexpr Index fileOf(Index square) {
		return square % RANK_SIZE;
	}

	template <bool whiteToMove>
	static constexpr inline bitboard enPassantRank() {
		if constexpr (whiteToMove) return RANK5;
		else return RANK4;
	}

	template <bool whiteToMove>
	static constexpr inline bitboard promotionRank() {
		if constexpr (whiteToMove) return RANK8;
		else return RANK1;
	}
	
	// ------------- Bit Operations -------------
	inline void printBitboard(bitboard bitboard, Index piece = -1ULL, std::string_view c = "1") {
		std::string toPrint = "";

		for (int rank = Chess::RANK_SIZE - 1; rank >= 0; rank--) {
			for (int file = 0; file < Chess::FILE_SIZE; file++) {
				int Index = rank * Chess::RANK_SIZE + file;

				if (Index == piece) {
					toPrint += "X ";
					continue;
				}
				toPrint += ((bitboard & Constants::SQUARE_BBS[Index]) != 0) ? c : "_";
				toPrint += " ";

			}
			toPrint += "\n";
		}

		std::cout << toPrint << std::endl;
	}

	constexpr Index index64[64] = {
		 0,  1, 48,  2, 57, 49, 28,  3,
		61, 58, 50, 42, 38, 29, 17,  4,
		62, 55, 59, 36, 53, 51, 43, 22,
		45, 39, 33, 30, 24, 18, 12,  5,
		63, 47, 56, 27, 60, 41, 37, 16,
		54, 35, 52, 21, 44, 32, 23, 11,
		46, 26, 40, 15, 34, 20, 31, 10,
		25, 14, 19,  9, 13,  8,  7,  6
	};

	inline [[nodiscard]] Index lsb(bitboard bb) {
		if (bb == 0) return -1;
		return index64[((bb & (0ULL - bb)) * 0x03f79d71b4cb0a89ULL) >> 58];
	}
	inline [[nodiscard]] Index popLSB(bitboard& bb) {
		if (bb == 0) return -1;
		Index Index = lsb(bb);
		bb &= (bb - 1);

		return Index;
	}
	inline [[nodiscard]] Index numOfBits(bitboard bb) {
		return std::popcount(bb);
	}

	static inline constexpr void setBit(bitboard& board, Index index) {
		board |= Constants::SQUARE_BBS[index];
	}

	static inline constexpr bitboard withBit(bitboard board, Index index) {
		return board | Constants::SQUARE_BBS[index];
	}

	static constexpr void removeBitAt(bitboard& board, Index index) {
		board &= ~Constants::SQUARE_BBS[index];
	}

	static inline constexpr bool hasBitAt(bitboard board, Index index) {
		return (board & Constants::SQUARE_BBS[index]) != 0;
	}

	// ------------- Moves -------------
	template<bool whiteToMove>
	constexpr bitboard pawnForward(bitboard bb) {
		if constexpr (whiteToMove) return bb << 8;
		else return bb >> 8;
	}

	template<bool whiteToMove>
	constexpr bitboard pawnBackward(bitboard bb) {
		if constexpr (whiteToMove) return bb >> 8;
		else return bb << 8;
	}

	template<bool whiteToMove>
	constexpr bitboard pawnForward2(bitboard bb) {
		if constexpr (whiteToMove) return bb << 16;
		else return bb >> 16;
	}

	template<bool whiteToMove>
	constexpr bitboard pawnBackward2(bitboard bb) {
		if constexpr (whiteToMove) return bb >> 16;
		else return bb << 16;
	}

	template<bool whiteToMove>
	constexpr bitboard pawnAttackLeft(bitboard bb) {
		if constexpr (whiteToMove) return bb << 7;
		else return bb >> 7;
	}

	template<bool whiteToMove>
	constexpr bitboard pawnAttackRight(bitboard bb) {
		if constexpr (whiteToMove) return bb << 9;
		else return bb >> 9;
	}

	template<bool whiteToMove>
	constexpr bitboard pawnLeftMask() {
		if constexpr (whiteToMove) return ~FILE_A;
		else return ~FILE_H;
	}

	template<bool whiteToMove>
	constexpr bitboard pawnRightMask() {
		if constexpr (whiteToMove) return ~FILE_H;
		else return ~FILE_A;
	}

	template<bool whiteToMove>
	constexpr bitboard lastPawnRank() {
		if constexpr (whiteToMove) return RANK7;
		else return RANK2;
	}

	template<bool whiteToMove>
	constexpr bitboard firstPawnRank() {
		if constexpr (whiteToMove) return RANK2;
		else return RANK7;
	}

	template<bool whiteToMove>
	inline constexpr bitboard backRank() {
		if constexpr (whiteToMove) return RANK1;
		else return RANK8;
	}

	constexpr Move NULL_MOVE;

}; 

struct Move {
	Index from, to;
	Piece piece;
	Flag flag;
	Move() : from(0), to(0), piece(0), flag(0) {}
	Move(Index from, Index to, Piece piece, Flag flag) : from(from), to(to), piece(piece), flag(flag) {}
};

struct Square {
	constexpr static File NULL_FILE = -1;
	constexpr static Rank NULL_RANK = -1;
	constexpr static Index NULL_INDEX = -1;

	File file;
	Rank rank;
	Index index;

	constexpr Square() : file(NULL_FILE), rank(NULL_RANK), index(NULL_INDEX) {}
	constexpr Square(File file, Rank rank) : file(file), rank(rank), index(rank * Chess::RANK_SIZE + file) {}
	Square(Index index) : file(index % Chess::RANK_SIZE), rank(index / Chess::RANK_SIZE), index(index) {}

	inline bool isValid() const {
		return this->file >= 0 && this->file < Chess::FILE_SIZE && this->rank >= 0 && this->rank < Chess::RANK_SIZE;
	}

	inline void addSquares(Square square) {
		this->rank += square.rank;
		this->file += square.file;
		this->index = this->rank * Chess::RANK_SIZE + this->file;
	}

	static inline Index getIndex(std::string_view notation) {
		File file = notation[0] - 'a';
		Rank rank = notation[1] - '1';
		return rank * Chess::RANK_SIZE + file;
	}

	static inline std::string getNotation(Index index) {
		Square s = Square(index);

		if (!s.isValid()) return "-";

		return std::string(1, 'a' + s.file) + std::string(1, '1' + s.rank);
	}
};

#endif

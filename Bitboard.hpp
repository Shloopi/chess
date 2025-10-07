#include <cstdint>
#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <string>
#include "Chess.hpp"
#include <iostream>
#include <bit>
#include <intrin.h>
#include "Constants.hpp"

using bitboard = uint64_t; // 64-bit unsigned integer to represent the bitboard.
using bit = uint64_t; // single bit
using Index = int64_t; // index of a square on the board (0-63).

namespace Bitboard {
	constexpr bitboard MAX_BITBOARD = 0xFFFFFFFFFFFFFFFFULL;

	constexpr bitboard RANK0 = 0x00000000000000FFULL;
	constexpr bitboard RANK1 = 0x000000000000FF00ULL;
	constexpr bitboard RANK2 = 0x0000000000FF0000ULL;
	constexpr bitboard RANK3 = 0x00000000FF000000ULL;
	constexpr bitboard RANK4 = 0x000000FF00000000ULL;
	constexpr bitboard RANK5 = 0x0000FF0000000000ULL;
	constexpr bitboard RANK6 = 0x00FF000000000000ULL;
	constexpr bitboard RANK7 = 0xFF00000000000000ULL;

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
		return index64[((bb & (0ULL - bb)) * 0x03f79d71b4cb0a89ULL) >> 58];
	}
	inline [[nodiscard]] Index popLSB(bitboard& bb) {
		Index Index = lsb(bb);
		bb &= (bb - 1);

		return Index;
	}
	inline [[nodiscard]] Index numOfBits(bitboard bb) {
		// get number of bits.

		if (bb == 0) return 0;

		#if defined(_MSC_VER)  // MSVC (Windows)
				return std::popcount(bb);

		#elif defined(__GNUC__) || defined(__clang__)  // GCC or Clang
				return __builtin_popcountll(bb);
		#else
				// fallback: simple pop count.
				bitIndex count = 0;
				while (bb) {
					bb &= (bb - 1);
					count++;
				}
				return count;
		#endif
	}

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
}

#endif
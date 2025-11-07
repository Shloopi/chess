#ifndef PSEUDO_MOVE_GEN_HPP
#define PSEUDO_MOVE_GEN_HPP

#include <array>
#include <vector>
#include "Chess.hpp"

namespace Directions {
	constexpr inline std::array<Square, 4> bishopDirections = { Square(1, 1), Square(1, -1), Square(-1, 1), Square(-1, -1) };
	constexpr inline std::array<Square, 4> rookDirections = { Square(1, 0), Square(-1, 0), Square(0, 1), Square(0, -1) };
}

namespace MagicGen {
	inline std::array<std::vector<bitboard>, Chess::BOARD_SIZE> bishopMagicTable;
	inline std::array<std::vector<bitboard>, Chess::BOARD_SIZE> rookMagicTable;

	inline uint64_t genMagicIndex(bitboard bb, bitboard magicNumber, Index shifter) { return (bb * magicNumber) >> shifter; }
	void genOccupancies(std::vector<bitboard>& occupancies, bitboard moves);
	bitboard genSlidingAttack(bool forBishop, Square sourceSquare, bitboard occupancy);
	void genMagicTable(bool forBishop);
}

namespace PseudoMoveGen {
	inline void init() {
		MagicGen::genMagicTable(true);
		MagicGen::genMagicTable(false);
	}

	template <bool whiteToMove>
	static bitboard getPawnCaptures(bitboard pawns) {
		return (Chess::pawnAttackLeft<whiteToMove>(pawns) & Chess::pawnLeftMask<whiteToMove>()) |
			(Chess::pawnAttackRight<whiteToMove>(pawns) & Chess::pawnRightMask<whiteToMove>());
	}

	inline bitboard getPseudoBishopMoves(bitboard enemyOrEmpty, Index index, bitboard occupancy) {
		return MagicGen::bishopMagicTable[index][MagicGen::genMagicIndex(
			occupancy & Constants::BISHOP_ATTACKS_NO_LAST_SQUARE[index],
			Constants::BISHOP_MAGIC_NUMBERS[index],
			Constants::BISHOP_SHIFTERS[index]
		)] & enemyOrEmpty;
	}

	inline bitboard getPseudoRookMoves(bitboard enemyOrEmpty, Index index, bitboard occupancy) {
		return MagicGen::rookMagicTable[index][MagicGen::genMagicIndex(
			occupancy & Constants::ROOK_ATTACKS_NO_LAST_SQUARE[index],
			Constants::ROOK_MAGIC_NUMBERS[index],
			Constants::ROOK_SHIFTERS[index]
		)] & enemyOrEmpty;
	}

	inline bitboard getPseudoQueenMoves(bitboard enemyOrEmpty, Index index, bitboard occupancy) {
		return PseudoMoveGen::getPseudoBishopMoves(enemyOrEmpty, index, occupancy) | PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, index, occupancy);
	}

	inline bitboard getPseudoKnightMoves(bitboard enemyOrEmpty, Index index) {
		return Constants::KNIGHT_MOVES[index] & enemyOrEmpty;
	}

	inline bitboard getPseudoKingMoves(bitboard enemyOrEmpty, Index index) {
		return Constants::KING_MOVES[index] & enemyOrEmpty;
	}
}


#endif
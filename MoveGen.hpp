#ifndef MOVE_GEN_HPP
#define MOVE_GEN_HPP

#include <array>
#include <vector>
#include "Bitboard.hpp"
#include "Chess.hpp"
#include "Board.hpp"

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
namespace MoveGen {
	inline void init() {
		MagicGen::genMagicTable(true);
		MagicGen::genMagicTable(false);
	}
	bitboard movesLegalityWhileChecked(const Board& board, Index sourceSquare, bitboard targetSquares);
	bitboard reducePinnedPiecesMoves(const Board& board, Index sourceSquare, bitboard targetSquares);
	bitboard genBitboardLegalPawnMoves(const Board& board, bitboard pawns);
	void genBitboardsLegalPawnMoves(const Board& board, bitboard pawns, bitboard& singlePushes, bitboard& doublePushes, bitboard& leftCaptures, bitboard& rightCaptures);
	template <Piece T>
	bitboard genLegalMoves(const Board& board, Index square);
	template<Piece T>
	MoveType calcMoveType(Index sourceSquare, Index targetSquare, const bitboard& enemyPieces);
	template <Piece T>
	void insertMoves(const Board& board, Move* moves, unsigned short& moveCount, Index sourceSquare, bitboard moveBitboard);
	void insertPawnMoves(const Board& board, Move* moves, unsigned short& moveCount, bitboard moveBitboard, short StartSquareDelta, MoveType moveType);
	void genLegalPawnMoves(const Board& board, bitboard pawns, Move* moves, unsigned short& moveCount);
	unsigned short genAllLegalMoves(const Board& board, Move* moves);
	void genLegalHumanMoves(const Board& board, Index square, Move* moves, unsigned short& moveCount);
	bitboard genLegalKingMoves(const Board& board, Index square);
	bool enPassantExposeKing(const Board& board, bitboard enPassantTarget, bitboard capturingPawn);
	inline bitboard calcSinglePawnPushes(const bitboard& pawns, const bitboard& emptySquares, const bool whiteToMove) { return (whiteToMove ? (pawns << 8) : (pawns >> 8)) & emptySquares; }
	inline bitboard calcDoublePawnPushes(const bitboard& singlePushes, const bitboard& emptySquares, const bool whiteToMove) { return whiteToMove ? (singlePushes << 8) & emptySquares & Bitboard::RANK3 : (singlePushes >> 8) & emptySquares & Bitboard::RANK4; }
	inline bitboard calcLeftPawnCaptures(const bitboard& pawns, const bitboard& captureSquares, const bool whiteToMove) { return whiteToMove ? ((pawns & (~Bitboard::FILE_A)) << 7) & captureSquares : ((pawns & (~Bitboard::FILE_A)) >> 9) & captureSquares; }
	inline bitboard calcRightPawnCaptures(const bitboard& pawns, const bitboard& captureSquares, const bool whiteToMove) { return whiteToMove ? (((pawns & (~Bitboard::FILE_H)) << 9) & captureSquares) : (((pawns & (~Bitboard::FILE_H)) >> 7) & captureSquares); }

	static bitboard getPawnCaptures(bitboard pawns, bool whiteToMove) {
		return MoveGen::calcLeftPawnCaptures(pawns, Bitboard::MAX_BITBOARD, whiteToMove) |
			MoveGen::calcRightPawnCaptures(pawns, Bitboard::MAX_BITBOARD, whiteToMove);
	}
	inline bitboard getPseudoBishopMoves(const Board& board, Index index, bitboard occupancy) {
		return MagicGen::bishopMagicTable[index][MagicGen::genMagicIndex(
			occupancy & Constants::BISHOP_ATTACKS_NO_LAST_SQUARE[index],
			Constants::BISHOP_MAGIC_NUMBERS[index],
			Constants::BISHOP_SHIFTERS[index]
		)] & ~board.getFriendlyPieces();
	}
	inline bitboard getPseudoRookMoves(const Board& board, Index index, bitboard occupancy) {
		return MagicGen::rookMagicTable[index][MagicGen::genMagicIndex(
			occupancy & Constants::ROOK_ATTACKS_NO_LAST_SQUARE[index],
			Constants::ROOK_MAGIC_NUMBERS[index],
			Constants::ROOK_SHIFTERS[index]
		)] & ~board.getFriendlyPieces();
	}

	inline bitboard getPseudoQueenMoves(const Board& board, Index index, bitboard occupancy) {
		return getPseudoBishopMoves(board, index, occupancy) | getPseudoRookMoves(board, index, occupancy);
	}

	inline bitboard getPseudoKnightMoves(const Board& board, Index index) {
		return Constants::KNIGHT_MOVES[index] & ~board.getFriendlyPieces();
	}

	inline bitboard getPseudoKingMoves(const Board& board, Index index) {
		return Constants::KING_MOVES[index] & ~board.getFriendlyPieces();
	}
}


#endif
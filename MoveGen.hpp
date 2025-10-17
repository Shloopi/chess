#ifndef MOVE_GEN_HPP
#define MOVE_GEN_HPP

#include <array>
#include <vector>
#include "Chess.hpp"
#include "Board.hpp"
#include "BoardState.hpp"

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
	template <bool whiteToMove>
	bitboard movesLegalityWhileChecked(const BoardState& state, Index sourceSquare, bitboard targetSquares);

	template <bool whiteToMove>
	bitboard reducePinnedPiecesMoves(const BoardState& state, Index sourceSquare, bitboard targetSquares);

	template <bool whiteToMove>
	bool enPassantExposeKing(const Board& board, bitboard enPassantTarget, bitboard capturingPawn);

	template <bool whiteToMove>
	bitboard genBitboardLegalPawnMoves(const BoardState& state, bitboard pawns);

	template <bool whiteToMove>
	void genBitboardsLegalPawnMoves(const BoardState& state, bitboard pawns, bitboard& singlePushes, bitboard& doublePushes, bitboard& leftCaptures, bitboard& rightCaptures);
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

	bool hasLegalMoves(const Board& board);

	template <bool whiteToMove>
	static bitboard getPawnCaptures(bitboard pawns) {
		return (Chess::pawnAttackLeft<whiteToMove>(pawns) & Chess::pawnLeftMask<whiteToMove>()) |
			(Chess::pawnAttackRight<whiteToMove>(pawns) & Chess::pawnRightMask<whiteToMove>());
	}

	template <bool whiteToMove>
	inline bitboard getPseudoBishopMoves(const Board& board, Index index, bitboard occupancy) {
		return MagicGen::bishopMagicTable[index][MagicGen::genMagicIndex(
			occupancy & Constants::BISHOP_ATTACKS_NO_LAST_SQUARE[index],
			Constants::BISHOP_MAGIC_NUMBERS[index],
			Constants::BISHOP_SHIFTERS[index]
		)] & board.notFriendlyPieces<whiteToMove>();
	}

	template <bool whiteToMove>
	inline bitboard getPseudoRookMoves(const Board& board, Index index, bitboard occupancy) {
		return MagicGen::rookMagicTable[index][MagicGen::genMagicIndex(
			occupancy & Constants::ROOK_ATTACKS_NO_LAST_SQUARE[index],
			Constants::ROOK_MAGIC_NUMBERS[index],
			Constants::ROOK_SHIFTERS[index]
		)] & board.notFriendlyPieces<whiteToMove>();
	}

	template <bool whiteToMove>
	inline bitboard getPseudoQueenMoves(const Board& board, Index index, bitboard occupancy) {
		return MoveGen::getPseudoBishopMoves<whiteToMove>(board, index, occupancy) | MoveGen::getPseudoRookMoves<whiteToMove>(board, index, occupancy);
	}

	template <bool whiteToMove>
	inline bitboard getPseudoKnightMoves(const Board& board, Index index) {
		return Constants::KNIGHT_MOVES[index] & board.notFriendlyPieces<whiteToMove>();
	}

	template <bool whiteToMove>
	inline bitboard getPseudoKingMoves(const Board& board, Index index) {
		return Constants::KING_MOVES[index] & board.notFriendlyPieces<whiteToMove>();
	}
}


#endif
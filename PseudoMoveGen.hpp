#ifndef PSEUDO_MOVE_GEN_HPP
#define PSEUDO_MOVE_GEN_HPP

#include <array>
#include <vector>
#include "Chess.hpp"
#include "Board.hpp"
#include "BoardState.hpp"


namespace PseudoMoveGen {
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

	template <bool whiteToMove, Piece piece>
	bitboard genLegalMoves(const BoardState& state, Index square);

	template<bool whiteToMove, Piece piece>
	Flag calcFlag(Index sourceSquare, Index targetSquare, const bitboard& enemyPieces);

	template <bool whiteToMove, Piece piece>
	void insertMoves(const Board& board, Move* moves, unsigned short& moveCount, Index sourceSquare, bitboard moveBitboard);

	template <bool whiteToMove>
	void insertPawnMoves(const Board& board, Move* moves, unsigned short& moveCount, bitboard moveBitboard, short StartSquareDelta, Flag flag);

	template <bool whiteToMove>
	void genLegalPawnMoves(const Board& board, bitboard pawns, Move* moves, unsigned short& moveCount);

	template <bool whiteToMove>
	unsigned short genAllLegalMoves(const BoardState& state, Move* moves);

	template <bool whiteToMove>
	void genLegalHumanMoves(const BoardState& state, Index square, Move* moves, unsigned short& moveCount);

	template <bool whiteToMove>
	bitboard genLegalKingMoves(const BoardState& state, Index square);

	template <bool whiteToMove>
	bool hasLegalMoves(const BoardState& state);

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
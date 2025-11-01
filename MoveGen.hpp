#ifndef MOVE_GEN_HPP
#define MOVE_GEN_HPP

#include <array>
#include <vector>
#include "Chess.hpp"
#include "Board.hpp"
#include "BoardState.hpp"
#include "PseudoMoveGen.hpp"

namespace MoveGen {
	inline void init() {
		PseudoMoveGen::init();
	}
	template <bool whiteToMove>
	uint16_t genAllLegalMoves(const BoardState& state, Move* moves);

	template <bool whiteToMove>
	uint16_t countAllLegalMoves(const BoardState& state);

	template <bool whiteToMove>
	bool hasLegalMoves(const BoardState& state);

	template <bool whiteToMove>
	bitboard movesLegalityWhileChecked(const BoardState& state, Index sourceSquare, bitboard targetSquares);

	template <bool whiteToMove>
	bitboard reducePinnedPiecesMoves(const BoardState& state, Index sourceSquare, bitboard targetSquares);

	template <bool whiteToMove, bool countOnly = false>
	void genPawnMoves(const BoardState& state, Move* moves, uint16_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genKingMoves(const BoardState& state, Move* moves, uint16_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genKnightMoves(const BoardState& state, Move* moves, uint16_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genBishopMoves(const BoardState& state, Move* moves, uint16_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genQueenMoves(const BoardState& state, Move* moves, uint16_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genRookMoves(const BoardState& state, Move* moves, uint16_t& moveCount);
}


#endif
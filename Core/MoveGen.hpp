#ifndef MOVE_GEN_HPP
#define MOVE_GEN_HPP

#include <array>
#include <vector>
#include "Chess.hpp"
#include "PseudoMoveGen.hpp"

class Game;

namespace MoveGen {
	inline void init() {
		PseudoMoveGen::init();
	}
	template <bool whiteToMove>
	uint8_t genAllLegalMoves(const Game& game, Move* moves);

	uint8_t genAllLegalMoves(const Game& game, Move* moves);

	template <bool whiteToMove>
	uint8_t countAllLegalMoves(const Game& game);

	bool hasLegalMoves(const Game& game, bool whiteToMove);

	template <bool whiteToMove>
	bool hasLegalMoves(const Game& game);

	template <bool whiteToMove>
	bitboard movesLegalityWhileChecked(const Game& game, Index sourceSquare, bitboard targetSquares);

	template <bool whiteToMove>
	bitboard reducePinnedPiecesMoves(const Game& game, Index sourceSquare, bitboard targetSquares);

	template <bool whiteToMove, bool countOnly = false>
	void genPawnMoves(const Game& game, Move* moves, uint8_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genKingMoves(const Game& game, Move* moves, uint8_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genKnightMoves(const Game& game, Move* moves, uint8_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genBishopMoves(const Game& game, Move* moves, uint8_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genQueenMoves(const Game& game, Move* moves, uint8_t& moveCount);

	template <bool whiteToMove, bool countOnly = false>
	void genRookMoves(const Game& game, Move* moves, uint8_t& moveCount);
}


#endif
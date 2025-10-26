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
	Flag calcFlag(Index sourceSquare, Index targetSquare);

	template <bool whiteToMove, Piece piece>
	void insertMoves(const Board& board, Move* moves, unsigned short& moveCount, Index sourceSquare, bitboard moveBitboard);

	template <bool whiteToMove>
	void insertPawnMoves(const Board& board, Move* moves, unsigned short& moveCount, bitboard moveBitboard, const Index StartSquareDelta, Flag flag);

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
	void genPawnMoves(const BoardState& state, Move* moves, uint8_t& moveCount);

	template <bool whiteToMove>
	void genKingMoves(const BoardState& state, Move* moves, uint8_t& moveCount);
}


#endif
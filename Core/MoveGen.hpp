#ifndef MOVE_GEN_HPP
#define MOVE_GEN_HPP

#include <array>
#include <vector>
#include "Chess.hpp"
#include "PseudoMoveGen.hpp"
#include "../Bot/BotConst.hpp"

class Game;

template <typename T = Move, std::size_t N = 218>
struct Moves {
	static_assert(
		std::is_same_v<T, Move> || std::is_same_v<T, EvalMove>,
		"Moves<T> can only be instantiated with Move or EvalMove"
		);

	T moves[N];
	uint8_t count;
	inline Moves() : count(0) {}
	inline void addMove(const T& move) {
		this->moves[this->count++] = move;
	}
	inline void init() {
		this->count = 0;
	}

	T* begin() {
		return moves;
	}

	T* end() {
		return moves + count;
	}

	const T* begin() const {
		return moves;
	}
	const T* end() const {
		return moves + count;
	}

	friend std::ostream& operator<<(std::ostream& os, const Moves& moves) {
		os << "Moves: " << (int)moves.count << '\n';

		for (const auto& move : moves) {
			os << move << '\n';
		}

		return os;
	}
};

namespace MoveGen {
	inline void init() {
		PseudoMoveGen::init();
	}
	template <bool whiteToMove>
	void genAllLegalMoves(const Game& game, Moves<>& moves);

	void genAllLegalMoves(const Game& game,Moves<>& moves);

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
	uint8_t genPawnMoves(const Game& game,Moves<>* moves = NULL);

	template <bool whiteToMove, bool countOnly = false>
	uint8_t genKingMoves(const Game& game,Moves<>* moves = NULL);

	template <bool whiteToMove, bool countOnly = false>
	uint8_t genKnightMoves(const Game& game,Moves<>* moves = NULL);

	template <bool whiteToMove, bool countOnly = false>
	uint8_t genBishopMoves(const Game& game,Moves<>* moves = NULL);

	template <bool whiteToMove, bool countOnly = false>
	uint8_t genQueenMoves(const Game& game,Moves<>* moves = NULL);

	template <bool whiteToMove, bool countOnly = false>
	uint8_t genRookMoves(const Game& game,Moves<>* moves = NULL);
}


#endif
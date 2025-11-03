#ifndef GAME_RESULT_HPP
#define GAME_RESULT_HPP

#include <cstdint>
#include "MoveGen.hpp"

enum class EndState : uint8_t {
	ONGOING,
	CHECKMATE,
	STALEMATE,
	DRAW_BY_INSUFFICIENT_MATERIAL,
	DRAW_BY_FIFTY_MOVE_RULE,
	DRAW_BY_THREEFOLD_REPETITION
};

namespace GameResult {
	template <bool whiteToMove>
	EndState checkState(const BoardState& state) {
		// no possible moves.
		if (!MoveGen::hasLegalMoves<whiteToMove>(state)) {
			if (state.inCheck()) {
				return EndState::CHECKMATE;
			}
			else {
				return EndState::STALEMATE;
			}
		}
		// check draw.
		else if (state.halfmoves == 50) {
			return EndState::DRAW_BY_FIFTY_MOVE_RULE;
		}
		// check insufficient material.
		else if (hasInsufficientMaterial<whiteToMove>(state) && hasInsufficientMaterial<!whiteToMove>(state)) {
			return EndState::DRAW_BY_INSUFFICIENT_MATERIAL;
		}
		/*else if (state.threefoldRepetition) {
			return EndState::DRAW_BY_THREEFOLD_REPETITION;
		}*/

		return EndState::ONGOING;
	}

	template <bool isWhite>
	inline bool hasInsufficientMaterial(const BoardState& state) {
		if (state.board.getPawns<isWhite>() == 0 && state.board.getQueens<isWhite>() == 0 &&
			state.board.getRooks<isWhite>() == 0) {
			bitboard bishopsAndKnights = state.board.getBishops<isWhite>() & state.board.getKnights<isWhite>();
			Index bitsNum = Chess::numOfBits(bishopsAndKnights);
			if (bitsNum < 2) {
				return true;
			}
			else if (bitsNum == 2 && bishopsAndKnights == state.board.getKnights<isWhite>()) {
				return true;
			}
		}

		return false;
	}

	
}




#endif
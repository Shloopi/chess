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
		//// check insufficient material.
		//else if (board.getFriendlyPieces(PieceType::PAWN) == 0ULL && board.getFriendlyPieces(PieceType::QUEEN) == 0ULL && 
		//        board.getFriendlyPieces(PieceType::ROOK) == 0ULL) {
		//    // TODO: add logic for checking insufficient material
		//    return state;
		//}
		/*else if (state.threefoldRepetition) {
			return EndState::DRAW_BY_THREEFOLD_REPETITION;
		}*/

		return EndState::ONGOING;
	}
}



#endif
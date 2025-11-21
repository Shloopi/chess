#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../Core/Chess.hpp"
#include "BotConst.hpp"
#include <functional>
#include "../Core/Game.hpp"

namespace Search {
	inline std::array<std::array<Move, 218>, 12> movesBuffer;
	inline std::array<std::array<EvalMove, 218>, 12> evalMovesBuffer;
	inline void MoveOrdering(EvalMove* moves, uint8_t moveCount, bool max = true);
	EvalMove iterativeDeepening(Game& game, Move* moves, uint8_t moveCount, uint8_t depth);
	Move search(Game& game, Move* moves, uint8_t moveCount, uint8_t depth);
}


#endif
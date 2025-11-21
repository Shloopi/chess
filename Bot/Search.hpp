#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../Core/Chess.hpp"
#include "BotConst.hpp"
#include <functional>
#include "../Core/Game.hpp"
#include "../Core/movegen.hpp"

namespace Search {
	inline std::array<Moves<>, 12> movesBuffer;
	inline std::array<Moves<EvalMove>, 12> evalMovesBuffer;
	inline void MoveOrdering(Moves<EvalMove>& moves, bool max = true);
	EvalMove iterativeDeepening(Game& game, Moves<>& moves, uint8_t depth);
	EvalMove search(Game& game, Moves<>& moves, uint8_t depth);
}


#endif
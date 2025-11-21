#include "Player.hpp"
#include "../Core/Game.hpp"
#include "../Core/Chess.hpp"
#include <chrono>
#include "BotConst.hpp"
#include "Search.hpp"

Move Bot::getBestMove(Game& game, Move* moves, uint8_t moveCount) {
	EvalMove best;
	//best = Search::iterativeDeepening(game, moves, moveCount, 4);
	//std::cout << best.move << '\n';
	best = Search::search(game, moves, moveCount, 3);
	//std::cout << best.move << '\n';

	return best.move;
}

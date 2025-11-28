#include "Player.hpp"
#include "../Core/Game.hpp"
#include "../Core/Chess.hpp"
#include <chrono>
#include "BotConst.hpp"
#include "Search.hpp"
#include "Evaluation.hpp"
Move Bot::getBestMove(Game& game, Moves<>& moves) {
	EvalMove best;
	//best = Search::iterativeDeepening(game, moves, 5);
	best = Search::search(game, moves, 4);

	return best.move;
}

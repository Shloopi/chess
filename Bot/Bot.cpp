#include "Bot.hpp"
#include <random>

Move Bot::getBestMove(const Game& game, Move* move, uint8_t moveCount) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, moveCount - 1);

	int value = dist(gen);

	return move[value];
}
#ifndef BOT_HPP
#define BOT_HPP

#include "../Core/Chess.hpp"
#include "../Core/Game.hpp"

class Bot {
public:
	Move getBestMove(const Game& game, Move* move, uint8_t moveCount);
	
};



#endif
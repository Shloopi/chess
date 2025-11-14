#ifndef INTERMEDIATE_HPP
#define INTERMEDIATE_HPP

#include "const.hpp"
#include "sdl2.hpp"
#include "../Core/Board.hpp"
#include "../Core/Game.hpp"

// Convert Chess Core Contexts into GUI Contexts.
namespace GUI {
	void convertBoard(const Board& board, GUI::GUIBoard& guiBoard);
	bool tryToMakeMove(Game& game, bool& whiteToMove, uint8_t moveCount, Move* moves, const GUI::GUIBoard& board, int mouseX, int mouseY);
};

#endif
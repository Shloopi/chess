#ifndef INTERMEDIATE_HPP
#define INTERMEDIATE_HPP

#include "const.hpp"
#include "sdl2.hpp"
#include "../Core/Board.hpp"

// Convert Chess Core Contexts into GUI Contexts.
namespace GUI {
	void convertBoard(const Board& board, GUI::GUIBoard& guiBoard);
};

#endif
#ifndef GUI_HPP
#define GUI_HPP

#include <SDL2/SDL.h>
#include "../Core/Chess.hpp"

namespace GUI {
	constexpr int TILE_SIZE = 80;
	constexpr int WIDTH = TILE_SIZE * Chess::BOARD_SIZE;
	constexpr int HEIGHT = TILE_SIZE * Chess::BOARD_SIZE;

	void drawChessBoard(SDL_Renderer* renderer);

};

#endif
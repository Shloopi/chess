#ifndef CONST_HPP
#define CONST_HPP

#include <array>
#include "../Core/Chess.hpp"
#include <unordered_map>
#include <SDL2/SDL.h>

namespace GUI {
	constexpr uint8_t TILE_SIZE = 100;
	constexpr int WIDTH = TILE_SIZE * Chess::RANK_SIZE;
	constexpr int HEIGHT = TILE_SIZE * Chess::RANK_SIZE;
	const std::string ASSETS_PATH = "GUI/assets/";
	struct Coord {
		int x, y;
		Coord() = default;
		Coord(int x, int y) : x(x), y(y) {}
	};
};


#endif
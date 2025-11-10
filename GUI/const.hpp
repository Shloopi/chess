#ifndef CONST_HPP
#define CONST_HPP

#include <array>
#include "../Core/Chess.hpp"
#include <unordered_map>
#include <SDL2/SDL.h>

namespace GUI {
	constexpr int TILE_SIZE = 100;
	constexpr int WIDTH = TILE_SIZE * Chess::RANK_SIZE;
	constexpr int HEIGHT = TILE_SIZE * Chess::RANK_SIZE;
	const std::string ASSETS_PATH = "GUI/assets/";
	using GUIBoard = std::array<std::array<char, Chess::RANK_SIZE>, Chess::FILE_SIZE>;
};


#endif
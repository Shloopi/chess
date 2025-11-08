#ifndef GUI_HPP
#define GUI_HPP
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "../Core/Chess.hpp"
#include "sdl2.hpp"
#include <unordered_map>

namespace GUI {
	constexpr int TILE_SIZE = 100;
	constexpr int WIDTH = TILE_SIZE * Chess::RANK_SIZE;
	constexpr int HEIGHT = TILE_SIZE * Chess::RANK_SIZE;
	const std::string ASSETS_PATH = "assets/";

	std::unordered_map<std::string, SDL_Texture*> pieces;

	void init();
	void drawChessBoard(SDL_Renderer* renderer);
	void mainLoop(App& app);

};

#endif
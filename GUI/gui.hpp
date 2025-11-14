#ifndef GUI_HPP
#define GUI_HPP
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "../Core/Chess.hpp"
#include "sdl2.hpp"
#include <unordered_map>
#include "../Core/MoveGen.hpp"
#include "const.hpp"
#include "intermediate.hpp"
#include "../Core/Game.hpp"

struct ClickedPiece {
	char piece;
	std::unique_ptr<SDL_Rect> clickedSquare;

	ClickedPiece() : piece('.'), clickedSquare(nullptr) {}
	inline bool isClicked() const {
		return this->clickedSquare != nullptr;
	}
};
class GuiApp {
private:
	std::unordered_map<char, SDL_Texture*> pieces;
	GUI::App app;
	GUIConverter conv;

	void init();
	void drawChessBoard();
	void drawPieces();
	void drawSquareHighlight();
	void showMoves();
public:
	GuiApp();
	void mainLoop(Game& game);

};

#endif
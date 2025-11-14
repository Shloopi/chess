#include <iostream>
#include "Core\MoveGen.hpp"
#include <iomanip>
#include "Core\BoardState.hpp"
#include "Test\Test.hpp"
#include "Utils\Zobrist.hpp"
#include "Utils\Fen.hpp"
#include <array>
#include "GUI/gui.hpp"

void init(Game& game) {
	Zobrist::init();
	MoveGen::init();
	Fen::handleFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", game);
	game.init();
}

int main() {
	Game game(true, false);
	init(game);

	//Test::showAfterMoveFens(game);
	//Test::perftPerMove(game, 1);
	//Test::makeMove(game, Square::getIndex("g7"), Square::getIndex("a1"));
	//Test::loopedTimedPerft(game, 1, 7);

	GuiApp app;
	app.mainLoop(game);
}
#include <iostream>
#include "Core\MoveGen.hpp"
#include <iomanip>
#include "Core\BoardState.hpp"
#include "Test\Test.hpp"
#include "Utils\Zobrist.hpp"
#include "Utils\Fen.hpp"
#include <array>
#include "GUI/gui.hpp"
#include "Bot/Evaluation.hpp"
#include "Test/Timer.hpp"

static void init(Game& game) {
	Zobrist::init();
	MoveGen::init();
	Evaluate::init();
	Fen::handleFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", game);
	//Fen::handleFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", game);

	
	game.init();
}
int main() {
	Game game(false, true);
	init(game);

	//Test::showAfterMoveFens(game);
	//Test::perftPerMove(game, 1);
	//Test::makeMove(game, Square::getIndex("g7"), Square::getIndex("a1"));
	//Test::loopedTimedPerft(game, 1, 6);

	GuiApp app;
	app.mainLoop(game);

}
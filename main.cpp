//#include <iostream>
//#include "Core\MoveGen.hpp"
//#include <iomanip>
//#include "Core\BoardState.hpp"
//#include "Test\Test.hpp"
//#include "Utils\Zobrist.hpp"
//#include "Utils\Fen.hpp"
//#include <array>
//
//bool init(Game& game) {
//	Zobrist::init();
//	MoveGen::init();
//	bool color = Fen::handleFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", game);
//	if (color) game.init<true, true>();
//	else game.init<false, true>();
//
//	return color;
//}
//int main() {
//	Game game;
//	bool color = init(game);
//
//	if (color) {
//		//Test::showAfterMoveFens<true>(game);
//		//Test::perftPerMove<true>(game, 1);
//		//Test::makeMove<true>(game, Square::getIndex("g7"), Square::getIndex("a1"));
//		Test::loopedTimedPerft<true>(game, 1, 7);
//	}
//	else {
//		//Test::showAfterMoveFens<false>(game);
//		//Test::perftPerMove<false>(game, 1);
//		//Test::makeMove<false>(game, Square::getIndex("g7"), Square::getIndex("a1"));
//		Test::loopedTimedPerft<false>(game, 1, 7);
//	}
//}
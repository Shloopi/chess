#include <iostream>
#include "MoveGen.hpp"
#include <iomanip>
#include "BoardState.hpp"
#include "Test.hpp"
#include "Zobrist.hpp"
#include "Fen.hpp"

int main() {
	Zobrist::init();
	MoveGen::init();
	BoardState state;
	Fen::handleFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", state);

	std::cout << state.board;
	
	for (int i = 6; i <= 6; i++) {
		test::timeDepth<true>(state, i);
	}

}
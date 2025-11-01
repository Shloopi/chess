#include <iostream>
#include "MoveGen.hpp"
#include <iomanip>
#include "BoardState.hpp"
#include "Test.hpp"
#include "Zobrist.hpp"
#include "Fen.hpp"
#include <array>

int main() {
	Zobrist::init();
	MoveGen::init();
	BoardState state;
	Fen::handleFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", state);
	//Fen::handleFen("rnbqkbnr/ppppp1pp/5p2/8/4P3/8/PPPPQPPP/RNB1KBNR b KQkq - 0 1", state);

	//std::cout << state;
	//std::array<Move, 218> moves;
	//uint8_t moveCount = MoveGen::genAllLegalMoves<true>(state, &moves[0]);	
	//std::cout << (int)moveCount << '\n';
	//for (int i = 0; i < moveCount; i++) {
	//	std::cout << moves[i];
	//}

	for (uint8_t i = 1; i <= 7; i++) {
		test::timeDepth<true>(state, i);
	}

}
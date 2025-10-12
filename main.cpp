#include <iostream>
#include "MoveGen.hpp"
#include <iomanip>
#include "Board.hpp"
#include "Test.hpp"

int main() {
	Zobrist::init();
	MoveGen::init();
	Board board;


	for (int i = 1; i < 6; i++) {
		uint64_t count = test::timeDepth(board, i);
		std::cout << i << " - " << count << '\n';
	}

}
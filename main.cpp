#include <iostream>
#include "MoveGen.hpp"
#include <iomanip>
#include "Board.hpp"

int main() {
	Zobrist::init();
	MoveGen::init();
	Board board;

	std::array<Move, 218> moves;

	unsigned short moveCount = MoveGen::genAllLegalMoves(board, &moves[0]);

	for (int i = 0; i < moveCount; i++) {
		std::cout << std::setw(3) << i + 1 << ". " << moves[i] << "\n";
	}
}
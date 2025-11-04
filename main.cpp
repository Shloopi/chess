#include <iostream>
#include "MoveGen.hpp"
#include <iomanip>
#include "BoardState.hpp"
#include "Test.hpp"
#include "Zobrist.hpp"
#include "Fen.hpp"
#include <array>

template <bool whiteToMove>
void getFens(Game& game) {
	std::array<Move, 218> moves;
	uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(game, &moves[0]);
	std::cout << '{';
	for (int i = 0; i < moveCount; i++) {
		game.makeMove<whiteToMove>(moves[i]);
		
		std::string a = Fen::genFen<!whiteToMove>(game);
		std::cout << '"' << a << '"';
		if (i + 1 < moveCount) std::cout << ", ";
	}
	std::cout << '}';
}

template <bool whiteToMove>
void getMovesDepth(Game& game, int depth) {
	std::array<Move, 218> moves;
	uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(game, &moves[0]);
	
	for (int i = 0; i < moveCount; i++) {
		const Move move = moves[i];
		GameSnapshot snapshot = game.createSnapshot();
		game.makeMove<whiteToMove>(move);
		std::cout << move << " - " << test::timeDepth<!whiteToMove>(game, depth, false) << '\n';
		game.undoMove<whiteToMove>(snapshot);
	}
}

template <bool whiteToMove>
void getMoves(Game& game) {
	std::array<Move, 218> moves;
	uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(game, &moves[0]);

	std::cout << "Moves: " << (int)moveCount << '\n';

	for (int i = 0; i < moveCount; i++) {
		const Move move = moves[i];
		std::cout << move << '\n';
	}
}

template <bool whiteToMove>
void timeDepth2(Game& game, int from, int to) {
	for (uint8_t i = from; i <= to; i++) {
		test::timeDepth<whiteToMove>(game, i);
	}
}

template <bool whiteToMove>
void makeMove(Game& game, Index startSquare, Index targetSquare) {
	std::array<Move, 218> moves;
	uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(game, &moves[0]);

	for (int i = 0; i < moveCount; i++) {
		const Move move = moves[i];

		if (move.from == startSquare && move.to == targetSquare) {
			std::cout << "making move - " << move << '\n';
			game.makeMove<whiteToMove>(move);
		}
	}
}

int main() {
	Zobrist::init();
	MoveGen::init();
	Game game;
	//Fen::handleFen("rnb1kbnr/pp1ppppp/2p5/q7/8/3P4/PPPNPPPP/R1BQKBNR w KQkq - 0 1", game);
	Fen::handleFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", game);
	const bool whiteToMove = true;
	game.init<whiteToMove>();

	//getFens<whiteToMove>(game);
	//getMovesDepth<whiteToMove>(game, 6);
	//getMoves<whiteToMove>(game);
	timeDepth2<whiteToMove>(game, 1, 7);


}
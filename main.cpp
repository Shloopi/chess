#include <iostream>
#include "MoveGen.hpp"
#include <iomanip>
#include "BoardState.hpp"
#include "Test.hpp"
#include "Zobrist.hpp"
#include "Fen.hpp"
#include <array>

template <bool whiteToMove>
void getFens(BoardState& state) {
	std::array<Move, 218> moves;
	uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(state, &moves[0]);
	std::cout << '{';
	for (int i = 0; i < moveCount; i++) {
		const Move move = moves[i];
		Board board = state.board.branch<whiteToMove>(move);
		BoardState state2 = state.branchState<!whiteToMove>(board);
		
		std::string a = Fen::genFen<!whiteToMove>(state2);
		std::cout << '"' << a << '"';
		if (i + 1 < moveCount) std::cout << ", ";
	}
	std::cout << '}';
}

template <bool whiteToMove>
void getMovesDepth(BoardState& state, int depth) {
	std::array<Move, 218> moves;
	uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(state, &moves[0]);
	
	for (int i = 0; i < moveCount; i++) {
		const Move move = moves[i];
		Board board = state.board.branch<whiteToMove>(move);
		BoardState state2 = state.branchState<!whiteToMove>(board);
		std::cout << move << " - " << test::timeDepth<!whiteToMove>(state2, depth) << '\n';
	}
}

template <bool whiteToMove>
void getMoves(BoardState& state) {
	std::array<Move, 218> moves;
	uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(state, &moves[0]);

	std::cout << "Moves: " << (int)moveCount << '\n';

	for (int i = 0; i < moveCount; i++) {
		const Move move = moves[i];
		std::cout << move << '\n';
	}
}

template <bool whiteToMove>
void timeDepth2(BoardState& state, int from, int to) {
	for (uint8_t i = 1; i <= 7; i++) {
		test::timeDepth<whiteToMove>(state, i);
	}
}

int main() {
	Zobrist::init();
	MoveGen::init();
	BoardState state;
	//Fen::handleFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", state);
	Fen::handleFen("rnbqkbnr/ppp1pppp/3p4/8/Q7/2P5/PP1PPPPP/RNB1KBNR b KQkq - 0 1", state);
	const bool whiteToMove = false;

	state.init<whiteToMove>();


	//getFens<whiteToMove>(state);
	//getMovesDepth<whiteToMove>(state, 1);
	getMoves<whiteToMove>(state);

}
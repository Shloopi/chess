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
		std::cout << move << " - " << test::timeDepth<!whiteToMove>(state2, depth, false) << '\n';
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
	for (uint8_t i = from; i <= to; i++) {
		test::timeDepth<whiteToMove>(state, i);
	}
}

template <bool whiteToMove>
BoardState makeMove(BoardState& state, Index startSquare, Index targetSquare) {
	std::array<Move, 218> moves;
	uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(state, &moves[0]);
	BoardState state2 = state;
	for (int i = 0; i < moveCount; i++) {
		const Move move = moves[i];

		if (move.from == startSquare && move.to == targetSquare) {
			std::cout << "making move - " << move << '\n';
			Board board = state.board.branch<whiteToMove>(move);
			state2 = state.branchState<!whiteToMove>(board);
		}
	}

	return state2;
}

int main() {
	Zobrist::init();
	MoveGen::init();
	BoardState state;
	//Fen::handleFen("rnb1kbnr/pp1ppppp/2p5/q7/8/3P4/PPPNPPPP/R1BQKBNR w KQkq - 0 1", state);
	Fen::handleFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", state);
	const bool whiteToMove = true;
	state.init<whiteToMove>();

	//getFens<whiteToMove>(state);
	//getMovesDepth<whiteToMove>(state, 1);
	//getMoves<whiteToMove>(state);
	timeDepth2<whiteToMove>(state, 1, 8);


}
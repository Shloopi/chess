#include "Search.hpp"
#include <algorithm>
#include "Evaluation.hpp"
#include "../Test/Timer.hpp"

inline static bool compareMoves(const EvalMove& move1, const EvalMove& move2) {
	return move1.score > move2.score;
}

static int negaMax(Game& game, int alpha, int beta, int depth, int& counter)
{
	counter++;

	if (depth == 0) return Evaluate::evaluate(game);

	std::array<Move, 218>& moves = Search::movesBuffer[depth];
	uint8_t moveCount = MoveGen::genAllLegalMoves(game, moves.data());
	int score, max = MIN_SCORE;

	for (uint8_t i = 0; i < moveCount; i++) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove(moves[i]);
		score = -negaMax(game, -beta, -alpha, depth - 1, counter);
		game.undoMove(snap);

		if (score > max) {
			max = score;
			if (score > alpha) {
				alpha = score;
			}
		}
		if (score >= beta) {
			return max;
		}
	}
	return max;
}
void Search::MoveOrdering(EvalMove* moves, uint8_t moveCount) {
	std::sort(moves, moves + moveCount - 1, compareMoves);
}

EvalMove Search::iterativeDeepening(Game& game, Move* moves, uint8_t moveCount, uint8_t depth) {
	Timer t("iterativeDeepening");

	std::array<EvalMove, 218>& evalMoves = Search::evalMovesBuffer[depth];
	int counter = 0;

	for (uint8_t i = 0; i < moveCount; i++) {
		evalMoves[i] = moves[i];
	}	
	for (uint8_t i = 1; i <= depth - 1; i++) {

		for (uint8_t j = 0; j < moveCount; j++) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove(evalMoves[j].move);

			evalMoves[j].score = -negaMax(game, MIN_SCORE, MAX_SCORE, i, counter);

			game.undoMove(snap);
		}
		MoveOrdering(evalMoves.data(), moveCount);
	}

	EvalMove best = moves[0];

	for (uint8_t i = 0; i < moveCount; i++) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove(evalMoves[i].move);

		evalMoves[i].score = -negaMax(game, MIN_SCORE, MAX_SCORE, depth, counter);

		if (evalMoves[i].score > best.score) {
			best = evalMoves[i];
		}

		game.undoMove(snap);
	}
	std::cout << "iterative - " << counter << " - ";
	return best;
}

Move Search::search(Game& game, Move* moves, uint8_t moveCount, uint8_t depth)
{
	Timer t("search");
	int counter = 0;
	Move best = moves[0];
	int score, max = MIN_SCORE;
	for (uint8_t i = 0; i < moveCount; i++) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove(moves[i]);

		score = -negaMax(game, MIN_SCORE, MAX_SCORE, depth, counter);

		if (score > max) {
			best = moves[i];
			max = score;
		}

		game.undoMove(snap);
	}

	std::cout << "search - " << counter << " - ";


	return best;
}


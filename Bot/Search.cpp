#include "Search.hpp"
#include <algorithm>
#include "Evaluation.hpp"
#include "../Test/Timer.hpp"

inline static bool compareMovesMax(const EvalMove& move1, const EvalMove& move2) {
	return move1.score > move2.score;
}
inline static bool compareMovesMin(const EvalMove& move1, const EvalMove& move2) {
	return move1.score < move2.score;
}
static int quiesce(Game& game, int alpha, int beta, int count)
{
	if (game.gameState == GameState::CHECKMATE) return MIN_SCORE;
	if (game.gameState != GameState::ONGOING) return 0;
	
	int eval = Evaluate::evaluate(game);
	int max = eval;
	
	if (max >= beta) return max;
	if (max > alpha) alpha = max;

	std::array<Move, 218>& moves = Search::movesBuffer[count];
	uint8_t moveCount = MoveGen::genAllLegalMoves(game, moves.data());
	int score;

	for (uint8_t i = 0; i < moveCount; i++) {
		if (moves[i].isCapture) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(moves[i]);
			score = -quiesce(game, -beta, -alpha, count + 1);
			game.undoMove(snap);

			if (score >= beta) return score;
			if (score > max) max = score;
			if (score > alpha) alpha = score;
		}
	}
	return max;
}

static int minimax(Game& game, int alpha, int beta, int depth)
{
	if (depth == 0 || game.gameState != GameState::ONGOING) return Evaluate::evaluate(game);

	std::array<Move, 218>& moves = Search::movesBuffer[depth];
	uint8_t moveCount = MoveGen::genAllLegalMoves(game, moves.data());
	int score;

	if (game.whiteToMove) {
		int max = MIN_SCORE;
		for (uint8_t i = 0; i < moveCount; i++) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(moves[i]);
			score = minimax(game, alpha, beta, depth - 1);
			game.undoMove(snap);

			if (score > max) max = score;
			if (score > alpha) alpha = score;
			if (beta <= alpha) break;
		}

		return max;
	}
	else {
		int min = MAX_SCORE;
		for (uint8_t i = 0; i < moveCount; i++) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(moves[i]);
			score = minimax(game, alpha, beta, depth - 1);
			game.undoMove(snap);

			if (score < min) min = score;
			if (score < beta) beta = score;
			if (beta <= alpha) break;
		}

		return min;
	}
}

void Search::MoveOrdering(EvalMove* moves, uint8_t moveCount, bool max) {
	std::sort(moves, moves + moveCount, max ? compareMovesMax : compareMovesMin);
}

EvalMove Search::iterativeDeepening(Game& game, Move* moves, uint8_t moveCount, uint8_t depth) {
	Timer t("iterativeDeepening");

	std::array<EvalMove, 218>& evalMoves = Search::evalMovesBuffer[depth];

	for (uint8_t i = 0; i < moveCount; i++) {
		evalMoves[i] = moves[i];
	}	
	for (uint8_t i = 1; i <= depth - 1; i++) {

		for (uint8_t j = 0; j < moveCount; j++) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(evalMoves[j].move);

			evalMoves[j].score = minimax(game, MIN_SCORE, MAX_SCORE, i);

			game.undoMove(snap);
		}
		MoveOrdering(evalMoves.data(), moveCount);
	}

	EvalMove best = moves[0];
	const bool isWhite = game.whiteToMove;

	for (uint8_t i = 0; i < moveCount; i++) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove<false, true>(evalMoves[i].move);

		evalMoves[i].score = minimax(game, MIN_SCORE, MAX_SCORE, depth);

		if (isWhite && evalMoves[i].score > best.score) {
			best = evalMoves[i];
		}
		else if (!isWhite && evalMoves[i].score < best.score) {
			best = evalMoves[i];
		}

		game.undoMove(snap);
	}
	return best;
}

Move Search::search(Game& game, Move* moves, uint8_t moveCount, uint8_t depth)
{
	Timer t("search");
	const bool isWhite = game.whiteToMove;
	int counter = 0;
	Move best = moves[0];
	int score, bestScore = MIN_SCORE;
	for (uint8_t i = 0; i < moveCount; i++) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove<false, true>(moves[i]);

		score = minimax(game, MIN_SCORE, MAX_SCORE, depth);

		if (isWhite && score > bestScore) {
			best = moves[i];
			bestScore = score;
		}
		else if (!isWhite && score < bestScore) {
			best = moves[i];
			bestScore = score;
		}

		game.undoMove(snap);
	}

	return best;
}


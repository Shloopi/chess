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

	Moves<>& moves = Search::movesBuffer[count];
	moves.init();
	MoveGen::genAllLegalMoves(game, moves);
	int score;

	for (auto& move : moves) {
		if (move.isCapture) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(move);
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

	Moves<>& moves = Search::movesBuffer[depth];
	moves.init();
	MoveGen::genAllLegalMoves(game, moves);
	int score;

	if (game.whiteToMove) {
		int max = MIN_SCORE;
		for (auto& move : moves)  {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(move);
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
		for (auto& move : moves) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(move);
			score = minimax(game, alpha, beta, depth - 1);
			game.undoMove(snap);

			if (score < min) min = score;
			if (score < beta) beta = score;
			if (beta <= alpha) break;
		}

		return min;
	}
}

void Search::MoveOrdering(Moves<EvalMove>& moves, bool max) {
	std::sort(moves.begin(), moves.end(), max ? compareMovesMax : compareMovesMin);
}

EvalMove Search::iterativeDeepening(Game& game, Moves<>& moves, uint8_t depth) {
	Timer t("iterativeDeepening");

	Moves<EvalMove>& evalMoves = Search::evalMovesBuffer[depth];
	evalMoves.init();

	for (const auto& move : moves) {
		evalMoves.addMove(EvalMove(move));
	}

	for (uint8_t i = 1; i <= depth - 1; i++) {
		for (auto& move : evalMoves) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(move.move);

			move.score = minimax(game, MIN_SCORE, MAX_SCORE, i);

			game.undoMove(snap);
		}
		MoveOrdering(evalMoves);
	}

	EvalMove best = evalMoves.moves[0];
	const bool isWhite = game.whiteToMove;

	for (auto& move : evalMoves) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove<false, true>(move.move);

		move.score = minimax(game, MIN_SCORE, MAX_SCORE, depth);

		if (isWhite && move.score > best.score) {
			best = move;
		}
		else if (!isWhite && move.score < best.score) {
			best = move;
		}

		game.undoMove(snap);
	}
	return best;
}

EvalMove Search::search(Game& game, Moves<>& moves, uint8_t depth) {
	Timer t("search");
	const bool isWhite = game.whiteToMove;
	int counter = 0;
	EvalMove best = moves.moves[0];
	int score;

	for (auto& move : moves) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove<false, true>(move);

		score = minimax(game, MIN_SCORE, MAX_SCORE, depth);

		if (isWhite && score > best.score) {
			best.move = move;
			best.score = score;
		}
		else if (!isWhite && score < best.score) {
			best.move = move;
			best.score = score;
		}

		game.undoMove(snap);
	}

	return best;
}


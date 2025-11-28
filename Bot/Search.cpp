#include "Search.hpp"
#include <algorithm>
#include "Evaluation.hpp"
#include "../Test/Timer.hpp"

//inline static bool compareMovesMax(const EvalMove& move1, const EvalMove& move2) {
//	return move1.score > move2.score;
//}
//inline static bool compareMovesMin(const EvalMove& move1, const EvalMove& move2) {
//	return move1.score < move2.score;
//}

//
//
//
//void Search::MoveOrdering(Moves<EvalMove>& moves, bool max) {
//	std::sort(moves.begin(), moves.end(), max ? compareMovesMax : compareMovesMin);
//}
//

inline static bool compareMoves(const EvalMove& move1, const EvalMove& move2) {
	return move1.score > move2.score;
}

template <bool captureOnly = false>
static void moveOrdering(Game& game, Moves<>& moves) {
	bitboard attacks = MoveGen::getAttackingSquares<Chess::PAWN>(game, !game.whiteToMove);

	Moves<EvalMove>& evalMoves = Search::evalMovesBuffer[0];
	evalMoves.init();

	for (const auto& move : moves) {
		if constexpr (captureOnly) {
			if (move.isCapture) {
				evalMoves.addMove(move);
			}
		}
		else {
			evalMoves.addMove(move);
		}
	}

	for (auto& move : evalMoves) {
		move.score = 0;

		if (move.move.isCapture) {
			Piece capturedPiece = game.board.getPieceAt(move.move.to);
			move.score += 10 * Evaluate::getPieceValue(capturedPiece) - Evaluate::getPieceValue(move.move.piece);
		}
		if (move.move.isPromotion()) {
			move.score += Evaluate::getPieceValue(move.move.getPromotedPiece());

		}
		if ((attacks & Constants::SQUARE_BBS[move.move.to]) != 0) {
			move.score -= Evaluate::getPieceValue(move.move.piece);
		}
	}

	std::sort(evalMoves.begin(), evalMoves.end(), compareMoves);

	moves.init();

	for (const auto& move : evalMoves) {
		moves.addMove(move.move);
	}
}

static int quiesce(Game& game, int alpha, int beta, int count)
{
	int score = Evaluate::evaluate(game);

	if (score >= beta) return beta;
	if (score > alpha) alpha = score;

	Moves<>& moves = Search::movesBuffer[count];
	moves.init();
	MoveGen::genAllLegalMoves(game, moves);
	moveOrdering<true>(game, moves);

	if (moves.count == 0) {
		return Evaluate::evaluate(game);
	}

	for (auto& move : moves) {
		if (move.isCapture) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(move);
			score = -quiesce(game, -beta, -alpha, count + 1);
			game.undoMove(snap);

			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}
	}
	return alpha;
}


static int minimax(Game& game, int alpha, int beta, int depth, int& counter) {
	if (depth == 0 || game.gameState != GameState::ONGOING) {
		counter++;
		//return quiesce(game, alpha, beta, 0);
		return Evaluate::evaluate(game);

	}
	Moves<>& moves = Search::movesBuffer[depth];
	moves.init();
	MoveGen::genAllLegalMoves(game, moves);
	moveOrdering(game, moves);

	int score;

	for (auto& move : moves) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove<false, true>(move);
		score = -minimax(game, -beta, -alpha, depth - 1, counter);
		game.undoMove(snap);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}
	return alpha;
}
EvalMove Search::search(Game& game, Moves<>& moves, uint8_t depth) {
	Timer t("search");

	moveOrdering(game, moves);

	EvalMove best = moves.moves[0];
	int score, counter = 0;

	for (auto& move : moves) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove<false, true>(move);
		score = -minimax(game, MIN_SCORE, MAX_SCORE, depth - 1, counter);
		game.undoMove(snap);

		//std::cout << score << " - " << move;
		if (score > best.score) {
			best.move = move;
			best.score = score;
		}
	}
	std::cout << "Moves: " << counter << '\n';

	return best;
}

EvalMove Search::iterativeDeepening(Game& game, Moves<>& moves, uint8_t depth) {
	Timer t("iterativeDeepening");

	Moves<EvalMove>& evalMoves = Search::evalMovesBuffer[depth];
	evalMoves.init();

	for (const auto& move : moves) {
		evalMoves.addMove(EvalMove(move));
	}
	int counter = 0;
	for (uint8_t i = 1; i <= depth - 2; i++) {
		for (auto& move : evalMoves) {
			GameSnapshot snap = game.createSnapshot();
			game.makeMove<false, true>(move.move);

			move.score = -minimax(game, MIN_SCORE, MAX_SCORE, depth - 2, counter);

			game.undoMove(snap);
		}

		std::sort(evalMoves.begin(), evalMoves.end(), compareMoves);
	}

	EvalMove best = evalMoves.moves[0];
	const bool isWhite = game.whiteToMove;

	for (auto& move : evalMoves) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove<false, true>(move.move);

		move.score = -minimax(game, MIN_SCORE, MAX_SCORE, depth - 1, counter);

		if (move.score > best.score) {
			best = move;
		}

		game.undoMove(snap);
	}

	std::cout << "Moves: " << counter << '\n';

	return best;
}

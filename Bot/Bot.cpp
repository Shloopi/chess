#include "Player.hpp"
#include "../Core/Game.hpp"
#include "../Core/Chess.hpp"

int Bot::evaluate(Game& game)
{
	int white = PAWN * Chess::numOfBits(game.board.getPawns<true>()) +
		KNIGHT * Chess::numOfBits(game.board.getKnights<true>()) +
		BISHOP * Chess::numOfBits(game.board.getBishops<true>()) +
		ROOK * Chess::numOfBits(game.board.getRooks<true>()) +
		QUEEN * Chess::numOfBits(game.board.getQueens<true>());

	int black = PAWN * Chess::numOfBits(game.board.getPawns<false>()) +
		KNIGHT * Chess::numOfBits(game.board.getKnights<false>()) +
		BISHOP * Chess::numOfBits(game.board.getBishops<false>()) +
		ROOK * Chess::numOfBits(game.board.getRooks<false>()) +
		QUEEN * Chess::numOfBits(game.board.getQueens<false>());
	
	return game.whiteToMove ? white - black : black - white;
}

int Bot::negaMax(Game& game, int depth)
{
	if (depth == 0) return this->evaluate(game);

	std::array<Move, 218>& moves = movesBuffer[depth];
	uint8_t moveCount = MoveGen::genAllLegalMoves(game, &moves[0]);

	int score, max = -99999;

	for (uint8_t i = 0; i < moveCount; i++) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove<true>(moves[i]);
		score = -negaMax(game, depth - 1);
		if (score > max)
			max = score;

		game.undoMove(snap);
	}
	return max;
}

Move Bot::getBestMove(Game& game, Move* moves, uint8_t moveCount) {
	int score, max = -99999;
	Move best = moves[0];
	for (uint8_t i = 0; i < moveCount; i++) {
		GameSnapshot snap = game.createSnapshot();
		game.makeMove<true>(moves[i]);
		score = -this->negaMax(game, 3);

		if (score > max) {
			max = score;
			best = moves[i];
		}
		game.undoMove(snap);
	}

	return best;
}

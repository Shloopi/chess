#include "Evaluation.hpp"
#include "../Test/Timer.hpp"
#include "BotConst.hpp"
#include "../Core/PseudoMoveGen.hpp"

inline static Index flipSquare(Index square) {
	return Chess::toIndex(Chess::RANK_SIZE - 1 - Chess::rankOf(square), Chess::fileOf(square));
}
namespace Evaluate {
	void init() {
		table.eraseTable();

		const bool WHITE = true;
		const bool BLACK = false;
		for (Piece piece = Chess::PAWN; piece <= Chess::KING; piece++) {
			for (Index square = 0; square < Chess::BOARD_SIZE; square++) {
				mg_tables[WHITE][piece][square] = mg_piece_tables[piece][square];
				eg_tables[WHITE][piece][square] = eg_piece_tables[piece][square];

				mg_tables[BLACK][piece][square] = mg_piece_tables[piece][flipSquare(square)];
				eg_tables[BLACK][piece][square] = eg_piece_tables[piece][flipSquare(square)];

			}
		}
	}
	template <bool isWhite, Piece piece>
	inline int evaluatePieceType(Game& game, int& mgScore, int& egScore, int& mgPhase) {
		Index square;
		int eval = 0;
		bitboard allPieces = game.board.getAllPieces();
		bitboard enemyOrEmpty = game.board.notFriendlyPieces<isWhite>();
		bitboard pieces = game.board.getPieces<isWhite, piece>();
		bitboard mobility;

		while (pieces != 0) {
			square = Chess::popLSB(pieces);

			mgScore += mg_value[piece] + mg_tables[isWhite][piece][square];
			egScore += eg_value[piece] + eg_tables[isWhite][piece][square];

			if constexpr (piece == Chess::KNIGHT || piece == Chess::BISHOP) mgPhase++;
			else if constexpr (piece == Chess::ROOK) mgPhase += 2;
			else if constexpr (piece == Chess::QUEEN) mgPhase += 4;

			// Mobility
			if constexpr (piece == Chess::KING) {
				mobility = PseudoMoveGen::getPseudoQueenMoves(enemyOrEmpty, square, allPieces);
			}
			else {
				mobility = PseudoMoveGen::getPseudoPieceMoves<piece>(enemyOrEmpty, square, allPieces);
			}

			eval += (coeff[piece] * log2(Chess::numOfBits(mobility) + 1));
		}

		return eval;
	}
	static int evaluatePlacement(Game& game) {
		std::array<int, 2> middleGame{ 0, 0 };
		std::array<int, 2> endGame{ 0, 0 };
		int mgPhase = 0;
		int whiteEval = 0;
		int blackEval = 0;

		blackEval += evaluatePieceType<false, Chess::PAWN>(game, middleGame[false], endGame[false], mgPhase);
		blackEval += evaluatePieceType<false, Chess::KNIGHT>(game, middleGame[false], endGame[false], mgPhase);
		blackEval += evaluatePieceType<false, Chess::BISHOP>(game, middleGame[false], endGame[false], mgPhase);
		blackEval += evaluatePieceType<false, Chess::ROOK>(game, middleGame[false], endGame[false], mgPhase);
		blackEval += evaluatePieceType<false, Chess::QUEEN>(game, middleGame[false], endGame[false], mgPhase);
		blackEval += evaluatePieceType<false, Chess::KING>(game, middleGame[false], endGame[false], mgPhase);

		whiteEval += evaluatePieceType<true, Chess::PAWN>(game, middleGame[true], endGame[true], mgPhase);
		whiteEval += evaluatePieceType<true, Chess::KNIGHT>(game, middleGame[true], endGame[true], mgPhase);
		whiteEval += evaluatePieceType<true, Chess::BISHOP>(game, middleGame[true], endGame[true], mgPhase);
		whiteEval += evaluatePieceType<true, Chess::ROOK>(game, middleGame[true], endGame[true], mgPhase);
		whiteEval += evaluatePieceType<true, Chess::QUEEN>(game, middleGame[true], endGame[true], mgPhase);
		whiteEval += evaluatePieceType<true, Chess::KING>(game, middleGame[true], endGame[true], mgPhase);

		int mgScore = middleGame[game.whiteToMove] - middleGame[!game.whiteToMove];
		int egScore = endGame[game.whiteToMove] - endGame[!game.whiteToMove];

		if (mgPhase > 24) mgPhase = 24;
		int egPhase = 24 - mgPhase;
		
		int eval = game.whiteToMove ? whiteEval - blackEval : blackEval - whiteEval;
		
		eval += (mgScore * mgPhase + egScore * egPhase) / 24;
	
		return eval;
	}
	inline static int mobilityScore(Game& game) {
		bitboard friendlyPieces = game.whiteToMove ? game.board.getAllPieces<true>() : game.board.getAllPieces<false>();
		bitboard enemyPieces = game.whiteToMove ? game.board.getAllPieces<true>() : game.board.getAllPieces<false>();
		int eval = 0;
		Index square;

		while (friendlyPieces != 0) {
			square = Chess::popLSB(friendlyPieces);

		}
	}
	int pawnIslands(bitboard pawns) {
		int counter = 0; // number of pawn islands.
		bool found = false; // if pawns were found in rank.
		bool counted = false; // if pawn island were already counted.

		bitboard iterator = Chess::FILE_A;

		for (int i = 0; i < Chess::RANK_SIZE; i++) {
			found = (iterator & pawns) != 0;

			if (!counted && found) {
				counter++;
			}
			counted = found;

			iterator <<= 1;
		}

		if (counter > 3) return -50;
		else if (counter == 3) return -20;
		else return 20;
	}
	int isolatedPawns(bitboard pawns, bitboard enemyPawns) {
		int counted = 0; // number of consecutive files with pawns.
		bitboard iterator = Chess::FILE_A;
		int eval = 0;

		for (int i = 0; i < Chess::RANK_SIZE; i++) {
			if ((iterator & pawns) != 0) {
				counted++;
			}
			else {
				if (counted == 1) {
					eval += isolatedPawnsRankTable[i];

					// if semi-open file.
					if (((iterator >> 1) & enemyPawns) == 0) eval += isolatedPawnsRankTable[i];
				}

				counted = 0;
			}

			iterator <<= 1;
		}

		return eval;
	}
	static int pawnStructures(Game& game) {
		bitboard friendlyPawns = game.whiteToMove ? game.board.getPawns<true>() : game.board.getPawns<false>();
		bitboard enemyPawns = !game.whiteToMove ? game.board.getPawns<true>() : game.board.getPawns<false>();

		int eval = 0;
		
		eval += pawnIslands(friendlyPawns) - pawnIslands(enemyPawns);
		eval += isolatedPawns(friendlyPawns, enemyPawns) - isolatedPawns(enemyPawns, friendlyPawns);


		return eval;
	}

	int evaluate(Game& game) {
		if (game.gameState == GameState::CHECKMATE) return MIN_SCORE;
		if (game.gameState != GameState::ONGOING) return 0;

		int eval = evaluatePlacement(game);
		eval += pawnStructures(game);


		return eval;
	}
}
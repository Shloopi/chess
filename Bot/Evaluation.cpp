#include "Evaluation.hpp"
#include "../Test/Timer.hpp"

inline static Index flipSquare(Index square) {
	return Chess::toIndex(Chess::RANK_SIZE - 1 - Chess::rankOf(square), Chess::fileOf(square));
}
namespace Evaluate {
	void init() {
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
	inline int evaluatePieceType(Board& board, int& mgScore, int& egScore) {
		int mgPhase = 0;
		Index square;

		bitboard pieces = board.getPieces<isWhite, piece>();

		while (pieces != 0) {
			square = Chess::popLSB(pieces);
			
			mgScore += mg_value[piece] + mg_tables[isWhite][piece][square];
			egScore += eg_value[piece] + eg_tables[isWhite][piece][square];
		
			if constexpr (piece == Chess::KNIGHT || piece == Chess::BISHOP) mgPhase++;
			else if constexpr (piece == Chess::ROOK) mgPhase += 2;
			else if constexpr (piece == Chess::QUEEN) mgPhase += 4;
		}

		return mgPhase;
	}
	static int evaluatePlacement(Game& game) {
		std::array<int, 2> middleGame{0, 0};
		std::array<int, 2> endGame{0, 0};
		int mgPhase = 0;

		mgPhase += evaluatePieceType<false, Chess::PAWN>(game.board, middleGame[false], endGame[false]);
		mgPhase += evaluatePieceType<false, Chess::KNIGHT>(game.board, middleGame[false], endGame[false]);
		mgPhase += evaluatePieceType<false, Chess::BISHOP>(game.board, middleGame[false], endGame[false]);
		mgPhase += evaluatePieceType<false, Chess::ROOK>(game.board, middleGame[false], endGame[false]);
		mgPhase += evaluatePieceType<false, Chess::QUEEN>(game.board, middleGame[false], endGame[false]);
		mgPhase += evaluatePieceType<false, Chess::KING>(game.board, middleGame[false], endGame[false]);

		mgPhase += evaluatePieceType<true, Chess::PAWN>(game.board, middleGame[true], endGame[true]);
		mgPhase += evaluatePieceType<true, Chess::KNIGHT>(game.board, middleGame[true], endGame[true]);
		mgPhase += evaluatePieceType<true, Chess::BISHOP>(game.board, middleGame[true], endGame[true]);
		mgPhase += evaluatePieceType<true, Chess::ROOK>(game.board, middleGame[true], endGame[true]);
		mgPhase += evaluatePieceType<true, Chess::QUEEN>(game.board, middleGame[true], endGame[true]);
		mgPhase += evaluatePieceType<true, Chess::KING>(game.board, middleGame[true], endGame[true]);

		int mgScore = middleGame[game.whiteToMove] - middleGame[!game.whiteToMove];
		int egScore = endGame[game.whiteToMove] - endGame[!game.whiteToMove];

		if (mgPhase > 24) mgPhase = 24;
		int egPhase = 24 - mgPhase;

		return (mgScore * mgPhase + egScore * egPhase) / 24;
	}

	int evaluate(Game& game) {
		return evaluatePlacement(game);
	}
}
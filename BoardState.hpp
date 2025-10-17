#ifndef BOARD_STATE_HPP
#define BOARD_STATE_HPP

#include <cstdint>
#include "Chess.hpp"
#include <unordered_map>
#include <stack>
#include "Board.hpp"
#include "MoveGen.hpp"

class BoardState {
private:
	template <bool whiteToMove>
	void calcChecks() {
		Index kingSquare = this->board.getKing<whiteToMove>();

		this->checkingPieces = 0ULL;

		// knight checks.
		this->checkingPieces |= Constants::KNIGHT_MOVES[kingSquare] & this->board.getKnights<!whiteToMove>();

		// bishop checks.
		this->checkingPieces |= MoveGen::getPseudoBishopMoves<whiteToMove>(board, kingSquare, this->allPieces) & this->board.getBishops<!whiteToMove>();

		// rook checks.
		this->checkingPieces |= MoveGen::getPseudoRookMoves<whiteToMove>(board, kingSquare, this->allPieces) & this->board.getRooks<!whiteToMove>();

		// queen checks.
		this->checkingPieces |= MoveGen::getPseudoQueenMoves<whiteToMove>(board, kingSquare, this->allPieces) & this->board.getQueens<!whiteToMove>();

		// pawn checks.
		this->checkingPieces |= MoveGen::getPawnCaptures<whiteToMove>(this->board.getPawns<!whiteToMove>()) & Constants::SQUARE_BBS[kingSquare];

		return this->checkingPieces;
	}

	template <bool whiteToMove>
	void calcPins() {
		Index kingSquare = this->board.getKing();
		bitboard allPieces = this->board.getAllPieces();

		// get pseudo moves from the king's square.
		bitboard bishopMoves = MoveGen::getPseudoBishopMoves<whiteToMove>(this->board, kingSquare, allPieces);
		bitboard rookMoves = MoveGen::getPseudoRookMoves<whiteToMove>(this->board, kingSquare, allPieces);
		bitboard queenMoves = MoveGen::getPseudoQueenMoves<whiteToMove>(this->board, kingSquare, allPieces);

		bitboard bishops = this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_BISHOP : ColoredPiece::WHITE_BISHOP];
		bitboard rooks = this->pieces[this->info.whiteMove ? ColoredPiece::WHITE_ROOK : ColoredPiece::BLACK_ROOK];
		bitboard queens = this->pieces[this->info.whiteMove ? ColoredPiece::WHITE_QUEEN : ColoredPiece::BLACK_QUEEN];

		// calculate the potential pieces that can pin pieces (treating queens as being both rooks and bishops).
		bitboard possiblePinningBishops = Constants::BISHOP_ATTACKS[kingSquare] & (bishops | queens) & ~this->info.checkingPieces;
		bitboard possiblePinningRooks = Constants::ROOK_ATTACKS[kingSquare] & (rooks | queens) & ~this->info.checkingPieces;

		// check pins for each type (bishops, rooks, queens).
		this->info.bishopPins = processPins<true>(possiblePinningBishops, bishopMoves);
		this->info.rookPins = processPins<false>(possiblePinningRooks, rookMoves);
	}

	template<bool forBishopPins>
	bitboard processPins(bitboard possiblePinningPieces, bitboard kingRayMoves) const {
		Index square;
		bitboard pinningPieceMoves, pins = 0ULL;

		// check for each potential pinning piece.
		while (possiblePinningPieces != 0) {
			square = Chess::lsb(possiblePinningPieces);
			possiblePinningPieces &= (possiblePinningPieces - 1);

			// get the pseudo moves of that piece.
			if constexpr (forBishopPins)
				pinningPieceMoves = MoveGen::getPseudoBishopMoves<whiteToMove>(this->board, square, this->allPieces);
			else
				pinningPieceMoves = MoveGen::getPseudoRookMoves<whiteToMove>(this->board, square, this->allPieces);

			// if the moves of the piece and the moves from the king overlapps, there is a pinned piece in between.
			pins |= (kingRayMoves & pinningPieceMoves);
		}

		return pins;
	}

public:
	Board board;

	uint8_t halfmoves;
	uint8_t fullmoves;

	bitboard stateKey;

	bitboard checkingPieces;
	bitboard rookPins;
	bitboard bishopPins;

	EndState endState;
	
	// count the number of repetitions of positions in the game.
	std::unordered_map<bitboard, char> positionsRep;
	bool threefoldRepetition;

	BoardState() : halfmoves(0), fullmoves(1), stateKey(0), checkingPieces(0), 
									rookPins(0), bishopPins(0), endState(EndState::ONGOING), threefoldRepetition(false) {}

	BoardState(Board board) : halfmoves(0), fullmoves(1), stateKey(0), checkingPieces(0),
		rookPins(0), bishopPins(0), endState(EndState::ONGOING), threefoldRepetition(false) {
		this->board = board;
	}

	template <bool whiteToMove>
	void init() {
		this->calcChecks<whiteToMove>();
		this->calcPins<whiteToMove>();
	}

	inline bool inCheck() const {
		return this->checkingPieces != 0ULL;
	}

	inline uint8_t numOfChecks() const {
		return Chess::numOfBits(this->checkingPieces);
	}

	inline bool isPiecePinned(Index square) const {
		return ((Constants::SQUARE_BBS[square] & (this->rookPins | this->bishopPins)) != 0ULL);
	}

	inline bitboard getPinnedPieces() const {
		return this->rookPins | this->bishopPins;
	}

	template <bool whiteToMove>
	bool isSquareAttacked(Index square, bitboard pieces) const {
		// knight attacks.
		if ((Constants::KNIGHT_MOVES[square] & this->board.getKnights<!whiteToMove>()) != 0) return true;

		// bishop attacks.
		if (MoveGen::getPseudoBishopMoves<whiteToMove>(this->board, square, pieces) & this->board.getBishops<!whiteToMove>()) != 0) return true;
		
		// rook attacks.
		if ((MoveGen::getPseudoRookMoves<whiteToMove>(this->board, square, pieces) & this->board.getRooks<!whiteToMove>()) != 0) return true;

		// queen attacks.
		if ((MoveGen::getPseudoQueenMoves<whiteToMove>(this->board, square, pieces) & this->board.getQueens<!whiteToMove>()) != 0) return true;

		// pawn attacks.
		if ((MoveGen::getPawnCaptures<!whiteToMove>(this->board.getPawns<!whiteToMove>()) & Constants::SQUARE_BBS[square]) != 0) return true;

		return false;
	}
};

#endif
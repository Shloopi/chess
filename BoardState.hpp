#ifndef BOARD_STATE_HPP
#define BOARD_STATE_HPP

#include <cstdint>
#include "Chess.hpp"
#include <unordered_map>
#include <stack>
#include "PseudoMoveGen.hpp"
#include "Board.hpp"

class BoardState {
private:
	template <bool whiteToMove>
	void calcChecks() {
		Index kingSquare = this->board.getKing<whiteToMove>();
		bitboard allPieces = this->board.getAllPieces();
		bitboard enemyOrEmpty = board.notFriendlyPieces<whiteToMove>();

		this->checkingPieces = 0ULL;

		// knight checks.
		this->checkingPieces |= Constants::KNIGHT_MOVES[kingSquare] & this->board.getKnights<!whiteToMove>();

		// bishop checks.

		// rook checks.
		this->checkingPieces |= (PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, kingSquare, allPieces) & this->board.getRooks<!whiteToMove>());

		// queen checks.
		this->checkingPieces |= (PseudoMoveGen::getPseudoQueenMoves(enemyOrEmpty, kingSquare, allPieces) & this->board.getQueens<!whiteToMove>());

		// pawn checks.
		this->checkingPieces |= (PseudoMoveGen::getPawnCaptures<!whiteToMove>(this->board.getPawns<!whiteToMove>()) & Constants::SQUARE_BBS[kingSquare]);
	}

	template <bool whiteToMove>
	void calcPins() {
		Index kingSquare = this->board.getKing<whiteToMove>();
		bitboard allPieces = this->board.getAllPieces();
		bitboard enemyOrEmpty = board.notFriendlyPieces<whiteToMove>();

		// get pseudo moves from the king's square.
		bitboard bishopMoves = PseudoMoveGen::getPseudoBishopMoves(enemyOrEmpty, kingSquare, allPieces);
		bitboard rookMoves = PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, kingSquare, allPieces);
		bitboard queenMoves = PseudoMoveGen::getPseudoQueenMoves(enemyOrEmpty, kingSquare, allPieces);

		bitboard bishops = this->board.getBishops<!whiteToMove>();
		bitboard rooks = this->board.getRooks<!whiteToMove>();
		bitboard queens = this->board.getQueens<!whiteToMove>();

		// calculate the potential pieces that can pin pieces (treating queens as being both rooks and bishops).
		bitboard possiblePinningBishops = Constants::BISHOP_ATTACKS[kingSquare] & (bishops | queens) & ~this->checkingPieces;
		bitboard possiblePinningRooks = Constants::ROOK_ATTACKS[kingSquare] & (rooks | queens) & ~this->checkingPieces;

		// check pins for each type (bishops, rooks, queens).
		this->bishopPins = this->processPins<whiteToMove, true>(possiblePinningBishops, bishopMoves);
		this->rookPins = this->processPins<whiteToMove, false>(possiblePinningRooks, rookMoves);
	}

	template<bool whiteToMove, bool forBishopPins>
	bitboard processPins(bitboard possiblePinningPieces, bitboard kingRayMoves) const {
		Index square;
		bitboard pinningPieceMoves, pins = 0ULL;
		bitboard enemyOrEmpty = board.notFriendlyPieces<whiteToMove>();

		// check for each potential pinning piece.
		while (possiblePinningPieces != 0) {
			square = Chess::lsb(possiblePinningPieces);
			possiblePinningPieces &= (possiblePinningPieces - 1);

			// get the pseudo moves of that piece.
			if constexpr (forBishopPins)
				pinningPieceMoves = PseudoMoveGen::getPseudoBishopMoves(enemyOrEmpty, square, this->board.getAllPieces());
			else
				pinningPieceMoves = PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, square, this->board.getAllPieces());

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

	inline bitboard getCheckingPieces() const {
		return this->checkingPieces;
	}

	inline uint8_t numOfChecks() const {
		return Chess::numOfBits(this->checkingPieces);
	}

	inline bool isPiecePinned(Index square) const {
		return ((Constants::SQUARE_BBS[square] & (this->rookPins | this->bishopPins)) != 0ULL);
	}

	inline bool isPinnedByBishop(Index square) const {
		return ((Constants::SQUARE_BBS[square] & this->bishopPins) != 0ULL);
	}

	inline bool isPinnedByRook(Index square) const {
		return ((Constants::SQUARE_BBS[square] & this->rookPins) != 0ULL);
	}

	inline bitboard getPinnedPieces() const {
		return this->rookPins | this->bishopPins;
	}

	template <bool whiteToMove>
	bool isSquareAttacked(Index square, bitboard pieces) const {
		bitboard enemyOrEmpty = board.notFriendlyPieces<whiteToMove>();

		// knight attacks.
		if ((Constants::KNIGHT_MOVES[square] & this->board.getKnights<!whiteToMove>()) != 0) return true;

		// bishop attacks.
		if ((PseudoMoveGen::getPseudoBishopMoves(enemyOrEmpty, square, pieces) & this->board.getBishops<!whiteToMove>()) != 0) return true;
		
		// rook attacks.
		if ((PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, square, pieces) & this->board.getRooks<!whiteToMove>()) != 0) return true;

		// queen attacks.
		if ((PseudoMoveGen::getPseudoQueenMoves(enemyOrEmpty, square, pieces) & this->board.getQueens<!whiteToMove>()) != 0) return true;

		// pawn attacks.
		if ((PseudoMoveGen::getPawnCaptures<!whiteToMove>(this->board.getPawns<!whiteToMove>()) & Constants::SQUARE_BBS[square]) != 0) return true;

		return false;
	}
	template <bool whiteToMove>
	BoardState branchState(Board& board) {
		BoardState state(board);
		state.halfmoves = this->halfmoves + (!whiteToMove ? 0 : 1);
		state.fullmoves = this->fullmoves + 1;
		state.init<whiteToMove>();

		return state;
	}
};

#endif
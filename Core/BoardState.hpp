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
	void calcChecks(const Board& board) {
		Index kingSquare = board.getKing<whiteToMove>();
		bitboard king = Constants::SQUARE_BBS[kingSquare];
		bitboard allPieces = board.getAllPieces();
		bitboard enemyOrEmpty = board.notFriendlyPieces<whiteToMove>();

		this->checkingPieces = 0ULL;

		// knight checks.
		this->checkingPieces |= (Constants::KNIGHT_MOVES[kingSquare] & board.getKnights<!whiteToMove>());

		// bishop checks.
		this->checkingPieces |= (PseudoMoveGen::getPseudoBishopMoves(enemyOrEmpty, kingSquare, allPieces) & board.getBishops<!whiteToMove>());

		// rook checks.
		this->checkingPieces |= (PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, kingSquare, allPieces) & board.getRooks<!whiteToMove>());

		// queen checks.
		this->checkingPieces |= (PseudoMoveGen::getPseudoQueenMoves(enemyOrEmpty, kingSquare, allPieces) & board.getQueens<!whiteToMove>());
		
		// pawn checks.
		this->checkingPieces |= (((Chess::pawnsRevAttackLeft<!whiteToMove>(king) & Chess::pawnLeftMask<!whiteToMove>()) |
			(Chess::pawnsRevAttackRight<!whiteToMove>(king) & Chess::pawnRightMask<!whiteToMove>())) & board.getPawns<!whiteToMove>());
	}

	template <bool whiteToMove>
	void calcPins(const Board& board) {
		Index kingSquare = board.getKing<whiteToMove>();
		bitboard allPieces = board.getAllPieces();
		bitboard enemyOrEmpty = board.notFriendlyPieces<whiteToMove>();

		// get pseudo moves from the king's square.
		bitboard bishopMoves = PseudoMoveGen::getPseudoBishopMoves(enemyOrEmpty, kingSquare, allPieces);
		bitboard rookMoves = PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, kingSquare, allPieces);

		bitboard bishops = board.getBishops<!whiteToMove>();
		bitboard rooks = board.getRooks<!whiteToMove>();
		bitboard queens = board.getQueens<!whiteToMove>();

		// calculate the potential pieces that can pin pieces (treating queens as being both rooks and bishops).
		bitboard possiblePinningBishops = Constants::BISHOP_ATTACKS[kingSquare] & (bishops | queens) & ~this->checkingPieces;
		bitboard possiblePinningRooks = Constants::ROOK_ATTACKS[kingSquare] & (rooks | queens) & ~this->checkingPieces;

		// check pins for each type (bishops, rooks, queens).
		this->bishopPins = this->processPins<whiteToMove, true>(board, kingSquare, possiblePinningBishops, bishopMoves);
		this->rookPins = this->processPins<whiteToMove, false>(board, kingSquare, possiblePinningRooks, rookMoves);

		// check pinned en passant.
		this->pinnedEnPassant = false;

		if (board.enPassant != 0 && (Constants::SQUARE_BBS[kingSquare] & Chess::enPassantRank<whiteToMove>()) != 0) {
			rookMoves = PseudoMoveGen::getPseudoRookMoves(Chess::enPassantRank<whiteToMove>(), kingSquare, allPieces);
			possiblePinningRooks &= Chess::enPassantRank<whiteToMove>();
			bitboard capturedPawn = Chess::pawnsBackward<whiteToMove>(board.enPassant);
			bitboard capturingPawn = Chess::pawnsRevAttackLeft<whiteToMove>(board.enPassant) & board.getPawns<whiteToMove>();

			if (capturingPawn == 0) capturingPawn = Chess::pawnsRevAttackRight<whiteToMove>(board.enPassant) & board.getPawns<whiteToMove>();
			
			if ((rookMoves & capturingPawn) != 0 || (rookMoves & capturedPawn) != 0) {
				while (possiblePinningRooks != 0) {
					Index square = Chess::popLSB(possiblePinningRooks);
					bitboard pinningPieceMoves = PseudoMoveGen::getPseudoRookMoves(Chess::enPassantRank<whiteToMove>(), square, allPieces);
					
					if ((pinningPieceMoves & capturingPawn) != 0 || (pinningPieceMoves & capturedPawn) != 0) {
						this->pinnedEnPassant = true;
						break;
					}
				}
			}
		}
	}

	template<bool whiteToMove, bool forBishopPins>
	bitboard processPins(const Board& board, Index kingSquare, bitboard possiblePinningPieces, bitboard kingRayMoves) const {
		Index square;
		bitboard betweenPieces, pins = 0ULL;
		bitboard enemyOrEmpty = board.notFriendlyPieces<whiteToMove>();
		bitboard allPieces = board.getAllPieces();
		bitboard friendlyPiece = board.getAllPieces<whiteToMove>();

		// check for each potential pinning piece.
		while (possiblePinningPieces != 0) {
			square = Chess::popLSB(possiblePinningPieces);
			betweenPieces = Constants::BETWEEN_PIECES_TABLE[kingSquare][square];

			// if there is exactly one piece in between, that piece is pinned.
			if (Chess::numOfBits(betweenPieces & allPieces) == 1) {
				pins |= (betweenPieces & friendlyPiece);
			}
		}

		return pins;
	}

public:
	bitboard checkingPieces;

	bitboard rookPins;
	bitboard bishopPins;
	bool pinnedEnPassant;

	BoardState() :	checkingPieces(0),rookPins(0), bishopPins(0), pinnedEnPassant(false) {}

	BoardState(bool whiteToMove, const Board& board)  {
		if (whiteToMove) this->init<true>(board);
		else this->init<false>(board);
	}

	inline void init(const Board& board, bool whiteToMove) {
		if (whiteToMove) this->init<true>(board);
		else this->init<false>(board);
	}

	template <bool whiteToMove>
	void init(const Board& board) {
		this->calcChecks<whiteToMove>(board);
		this->calcPins<whiteToMove>(board);
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

	friend std::ostream& operator<<(std::ostream& os, const BoardState& state) {
		os << "Checking Pieces: \n" << Chess::showBitboard(state.checkingPieces);
		os << "Rook Pins: \n" << Chess::showBitboard(state.rookPins);
		os << "Bishop Pins: \n" << Chess::showBitboard(state.bishopPins);
		os << "Pinned En Passant: " << state.pinnedEnPassant << '\n';
		return os;
	}
};

#endif
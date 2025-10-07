#ifndef BOARD_HPP
#define BOARD_HPP

#include "Bitboard.hpp"
#include <array>
#include <iostream>
#include "Zobrist.hpp"
#include "Fen.hpp"
#include <unordered_map>
#include <stack>
#include "BoardInfo.hpp"
#include "Piece.hpp"
#include "Move.hpp"
#include <functional>

class Board {
private:
	const static std::string defaultFen;

	BoardInfo info;
	bitboard checkingPieces;
	bitboard rookPins;
	bitboard bishopPins;

	bitboard boardKey;
	bitboard allPieces;
	bitboard whitePieces;
	bitboard blackPieces;

	std::array<bitboard, 12> pieces;
	std::array<ColoredPiece, Chess::BOARD_SIZE> piecesArr;

	// count the number of repetitions of positions in the game.
	std::unordered_map<bitboard, char> positionsRep;
	bool threefoldRepetition;

	// save last moves, positions and board infos.
	std::stack<Move> lastMoves;
	std::stack<bitboard> lastPositions;
	std::stack<BoardInfo> lastBoardInfos;

	void init(const std::string& fen) {
		Fen::handleFen(fen, this->pieces, this->info);
		this->updateAllPieces();
		this->updateBoardKey();
		this->buildPiecesArr();
	}
	void updateAllPieces() {
		this->allPieces = 0ULL;
		for (int i = 0; i < this->pieces.size(); i++) {
			if (i < 6) {
				this->whitePieces |= this->pieces[i];
			}
			else {
				this->blackPieces |= this->pieces[i];
			}
		}
		this->allPieces = this->whitePieces | this->blackPieces;
	}
	void updateBoardKey() {
		this->boardKey = Zobrist::genKey(this->info.whiteMove);
		for (int i = 0; i < this->pieces.size(); i++) {
			bitboard pieceBB = this->pieces[i];
			while (pieceBB) {
				Index square = Bitboard::popLSB(pieceBB);

				this->boardKey = Zobrist::applyPiece(this->boardKey, static_cast<Piece>(i), square);
			}
		}
		this->boardKey = Zobrist::applyBoardInfo(this->boardKey, this->info);
	}

	void calcPins();

	template<bool forBishopPins>
	bitboard processPins(bitboard possiblePinningPieces, bitboard kingRayMoves) const;

	void calcChecks();
	void buildPiecesArr();


public:
	Board() { this->init(Board::defaultFen); }
	Board(const std::string& fen) { this->init(fen); }
	inline bitboard getPieces(ColoredPiece piece) const { return this->pieces[piece]; }
	inline bitboard getPieces(Piece piece, bool white) const { return this->pieces[piece + (white ? 0 : 6)]; }
	inline bitboard getAllPieces() const { return this->allPieces; }
	inline bitboard getEnemyPieces() const { return this->info.whiteMove ? this->blackPieces : this->whitePieces; }
	inline bitboard getFriendlyPieces() const { return this->info.whiteMove ? this->whitePieces : this->blackPieces; }
	inline bitboard getPinnedPieces() const { return this->rookPins | this->bishopPins; }
	inline bool isPinnedByBishop(Index square) const { return (Constants::SQUARE_BBS[square] & this->bishopPins) != 0; }
	inline bool isPinnedByRook(Index square) const { return (Constants::SQUARE_BBS[square] & this->rookPins) != 0; }
	inline Index getEnPassantTarget() const { return this->info.enPassantTarget; }
	inline Index getCheckingPiecePos() const { return Bitboard::lsb(this->checkingPieces); }
	inline bool inCheck() const { return this->checkingPieces != 0; }
	inline bool whiteToMove() const { return this->info.whiteMove; }
	short numOfChecks() const { return Bitboard::numOfBits(this->checkingPieces); }
	inline bitboard getCheckingPieces() const { return this->checkingPieces; }
	inline bitboard isPiecePinned(Index square) const { return (Constants::SQUARE_BBS[square] & (this->rookPins | this->bishopPins)) != 0; }
	inline ColoredPiece getColoredPieceOnSquare(Index square) const {
		if (square > Chess::BOARD_SIZE) return ColoredPiece::COLORED_NONE;
		return this->piecesArr[square];
	}
	inline Piece getPieceOnSquare(Index square) const {
		return PieceHelper::getPieceType(this->getColoredPieceOnSquare(square));
	}
	friend std::ostream& operator<<(std::ostream& os, const Board& board);

	inline bool pieceOnSquare(Index square) const {
		return (this->allPieces & Constants::SQUARE_BBS[square]) != 0;
	}
	inline Index getKingPos() const { return Bitboard::lsb(this->pieces[this->info.whiteMove ? ColoredPiece::WHITE_KING : ColoredPiece::BLACK_KING]); }
	
	inline bool canCastleKingside() const { return this->info.canCastleRight(); }
	inline bool canCastleQueenside() const { return this->info.canCastleLeft(); }
	bool isSquareAttacked(Index square, bitboard pieces) const;


};

#endif
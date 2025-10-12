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
	// rook starting positions.
	static constexpr bit whiteLeftRook = 0b1ULL;
	static constexpr bit whiteRightRook = 0b10000000ULL;
	static constexpr bit blackLeftRook = 0b1ULL << 56;
	static constexpr bit blackRightRook = 0b10000000ULL << 56;

	// rook castling positions.
	static constexpr bit whiteLeftRookCastle = 0b1000ULL;
	static constexpr bit whiteRightRookCastle = 0b100000ULL;
	static constexpr bit blackLeftRookCastle = 0b1000ULL << 56;
	static constexpr bit blackRightRookCastle = 0b100000ULL << 56;

	const static std::string defaultFen;

	BoardInfo info;


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
		this->calcChecks();
		this->calcPins();
	}
	void updateAllPieces() {
		this->whitePieces = 0ULL;
		this->blackPieces = 0ULL;
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

				this->boardKey = Zobrist::applyPiece(this->boardKey, static_cast<ColoredPiece>(i), square);
			}
		}
		this->boardKey = Zobrist::applyBoardInfo(this->boardKey, this->info);
	}

	void calcPins();

	template<bool forBishopPins>
	bitboard processPins(bitboard possiblePinningPieces, bitboard kingRayMoves) const;

	void calcChecks();
	void buildPiecesArr();

	void removePiece(ColoredPiece piece, Index sourceSquare, bitboard& coloredPieces);
	void addPiece(ColoredPiece piece, Index sourceSquare, bitboard& coloredPieces);
	void movePiece(ColoredPiece piece, Index sourceSquare, Index targetSquare, bitboard& coloredPieces);

public:
	Board() { this->init(Board::defaultFen); }
	Board(const std::string& fen) { this->init(fen); }
	void makeMove(Move move);
	void unmakeMove();
	void checkState();
	bitboard getKey() const { return this->boardKey; }
	inline bitboard getPieces(ColoredPiece piece) const { return this->pieces[piece]; }
	inline bitboard getPieces(Piece piece, bool white) const { return this->pieces[piece + (white ? 0 : 6)]; }
	inline bitboard getAllPieces() const { return this->allPieces; }
	inline bitboard getEnemyPieces() const { return this->info.whiteMove ? this->blackPieces : this->whitePieces; }
	inline bitboard getFriendlyPieces() const { return this->info.whiteMove ? this->whitePieces : this->blackPieces; }
	inline bitboard getPinnedPieces() const { return this->info.rookPins | this->info.bishopPins; }
	inline bool isPinnedByBishop(Index square) const { return (Constants::SQUARE_BBS[square] & this->info.bishopPins) != 0; }
	inline bool isPinnedByRook(Index square) const { return (Constants::SQUARE_BBS[square] & this->info.rookPins) != 0; }
	inline Index getEnPassantTarget() const { return this->info.enPassantTarget; }
	inline Index getCheckingPiecePos() const { return Bitboard::lsb(this->info.checkingPieces); }
	inline bool inCheck() const { return this->info.checkingPieces != 0; }
	inline bool whiteToMove() const { return this->info.whiteMove; }
	short numOfChecks() const { return Bitboard::numOfBits(this->info.checkingPieces); }
	inline bitboard getCheckingPieces() const { return this->info.checkingPieces; }
	inline bitboard isPiecePinned(Index square) const { return (Constants::SQUARE_BBS[square] & (this->info.rookPins | this->info.bishopPins)) != 0; }
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

	// game over.
	[[nodiscard]] bool whiteWins() const { return this->info.endState == 'w'; }
	[[nodiscard]] bool blackWin() const { return this->info.endState == 'b'; }
	[[nodiscard]] bool isDraw() const { return this->info.endState == 'd'; }
	[[nodiscard]] bool isThreefoldRepetition() const { return this->threefoldRepetition; }
	[[nodiscard]] bool gameOver() const { return this->whiteWins() || this->blackWin() || this->isDraw(); }
};

#endif
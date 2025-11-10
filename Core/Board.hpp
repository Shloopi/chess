#ifndef BOARD_HPP
#define BOARD_HPP

#include <array>
#include <iostream>
#include <unordered_map>
#include <stack>
#include <functional>
#include "Constants.hpp"
#include "Chess.hpp"
#include "PseudoMoveGen.hpp"
#include <bitset>

class Board {
private:
	static constexpr uint8_t whiteShortCastleMask = 0b100;
	static constexpr uint8_t whiteLongCastleMask = 0b1000;
	static constexpr uint8_t blackShortCastleMask = 0b1;
	static constexpr uint8_t blackLongCastleMask = 0b10;

	static constexpr uint8_t longCastleMask = whiteLongCastleMask | blackLongCastleMask;
	static constexpr uint8_t shortCastleMask = whiteShortCastleMask | blackShortCastleMask;
	static constexpr uint8_t whiteCastleMask = whiteShortCastleMask | whiteLongCastleMask;
	static constexpr uint8_t blackCastleMask = blackShortCastleMask | blackLongCastleMask;

	static constexpr bitboard whiteQueensideRook = 0b1;
	static constexpr bitboard whiteKingsideRook = 0b10000000;
	static constexpr bitboard blackQueensideRook = whiteQueensideRook << 56;
	static constexpr bitboard blackKingsideRook = whiteKingsideRook << 56;

	static constexpr bitboard whiteRookShortCastle = 0b100000;
	static constexpr bitboard whiteRookLongCastle = 0b1000;
	static constexpr bitboard blackRookShortCastle = whiteRookShortCastle << 56;
	static constexpr bitboard blackRookLongCastle = whiteRookLongCastle << 56;

public:
	bitboard whitePawns{ 0 }, whiteKnights{ 0 }, whiteBishops{ 0 }, whiteRooks{ 0 }, whiteQueens{ 0 };
	bitboard blackPawns{ 0 }, blackKnights{ 0 }, blackBishops{ 0 }, blackRooks{ 0 }, blackQueens{ 0 };
	Index whiteKing{ 0 }, blackKing{ 0 };

	uint8_t castlingRights{ 0b1111 }; // white left, white right, black left, black right
	bitboard enPassant{ 0 };

	Board() = default;

	Board(bitboard wp, bitboard wn, bitboard wb, bitboard wr, bitboard wq, Index wk,
		bitboard bp, bitboard bn, bitboard bb, bitboard br, bitboard bq, Index bk, uint8_t castling, bitboard ep) :
		whitePawns(wp), whiteKnights(wn), whiteBishops(wb), whiteRooks(wr), whiteQueens(wq), whiteKing(wk),
		blackPawns(bp), blackKnights(bn), blackBishops(bb), blackRooks(br), blackQueens(bq), blackKing(bk), castlingRights(castling), enPassant(ep)
	{
	}

	template <bool whiteToMove>
	bool isSquareAttacked(Index square, bitboard pieces) const {
		bitboard enemyOrEmpty = this->notFriendlyPieces<whiteToMove>();
		bitboard squareBB = Constants::SQUARE_BBS[square];

		// knight attacks.
		if ((Constants::KNIGHT_MOVES[square] & this->getKnights<!whiteToMove>()) != 0) return true;

		// bishop attacks.
		if ((PseudoMoveGen::getPseudoBishopMoves(enemyOrEmpty, square, pieces) & this->getBishops<!whiteToMove>()) != 0) return true;

		// rook attacks.
		if ((PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, square, pieces) & this->getRooks<!whiteToMove>()) != 0) return true;

		// queen attacks.
		if ((PseudoMoveGen::getPseudoQueenMoves(enemyOrEmpty, square, pieces) & this->getQueens<!whiteToMove>()) != 0) return true;

		// King attacks.
		if (Constants::KING_MOVES[square] & Constants::SQUARE_BBS[this->getKing<!whiteToMove>()]) return true;

		// pawn attacks.
		if ((((Chess::pawnsRevAttackLeft<!whiteToMove>(squareBB) & Chess::pawnLeftMask<!whiteToMove>()) |
			(Chess::pawnsRevAttackRight<!whiteToMove>(squareBB) & Chess::pawnRightMask<!whiteToMove>())) & this->getPawns<!whiteToMove>()) != 0) return true;

		return false;
	}

	friend std::ostream& operator<<(std::ostream& os, const Board& board) {
		bitboard allPieces = board.getAllPieces();
		bitboard whitePieces = board.getAllPieces<true>();
		bitboard blackPieces = board.getAllPieces<false>();

		char c;
		for (int rank = Chess::RANK_SIZE - 1; rank >= 0; rank--) {
			for (int file = 0; file < Chess::FILE_SIZE; file++) {
				int Index = rank * Chess::RANK_SIZE + file;

				if (Constants::SQUARE_BBS[Index] & allPieces) {
					if (Constants::SQUARE_BBS[Index] & whitePieces) {
						c = Chess::getPiece<true>(board.getPieceAt<true>(Index));
					}
					else {
						c = Chess::getPiece<false>(board.getPieceAt<false>(Index));
					}
				}
				else {
					c = '_';
				}

				os << c << ' ';
			}
			os << '\n';
		}

		os << "En Passant: ";
		os << Square::getNotation(Chess::lsb(board.enPassant)) << '\n';
		os << "Castling Rights: " << std::bitset<4>(board.castlingRights) << "\n\n";
		return os;
	}

	template <bool whiteToMove>
	static constexpr inline bitboard startingRooks() {
		return Board::startingQueensideRook<whiteToMove>() | Board::startingKingsideRook<whiteToMove>();
	}

	template <bool whiteToMove>
	static constexpr inline bitboard startingQueensideRook() {
		if constexpr (whiteToMove) {
			return Board::whiteQueensideRook;
		}
		else {
			return Board::blackQueensideRook;
		}
	}

	template <bool whiteToMove>
	static constexpr inline bitboard startingKingsideRook() {
		if constexpr (whiteToMove) {
			return Board::whiteKingsideRook;
		}
		else {
			return Board::blackKingsideRook;
		}
	}

	template <bool whiteToMove>
	static constexpr inline bitboard getRookShortCastlingMove() {
		if constexpr (whiteToMove) {
			return Board::whiteKingsideRook | Board::whiteRookShortCastle;
		}
		else {
			return Board::blackKingsideRook | Board::blackRookShortCastle;
		}
	}

	template <bool whiteToMove>
	static constexpr inline bitboard getRookLongCastlingMove() {
		if constexpr (whiteToMove) {
			return Board::whiteQueensideRook | Board::whiteRookLongCastle;
		}
		else {
			return Board::blackQueensideRook | Board::blackRookLongCastle;
		}
	}

	template <bool whiteToMove>
	inline bitboard getPawns() const {
		if constexpr (whiteToMove) {
			return this->whitePawns;
		}
		else {
			return this->blackPawns;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getKnights() const {
		if constexpr (whiteToMove) return this->whiteKnights;
		else return this->blackKnights;
	}
		
	template <bool whiteToMove>
	constexpr inline bitboard getBishops() const {
		if constexpr (whiteToMove) {
			return this->whiteBishops;
		}
		else {
			return this->blackBishops;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getRooks() const {
		if constexpr (whiteToMove) {
			return this->whiteRooks;
		}
		else {
			return this->blackRooks;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getQueens() const {
		if constexpr (whiteToMove) {
			return this->whiteQueens;
		}
		else {
			return this->blackQueens;
		}
	}

	template <bool whiteToMove>
	constexpr inline Index getKing() const {
		if constexpr (whiteToMove) {
			return this->whiteKing;
		}
		else {
			return this->blackKing;
		}
	}

	template <bool whiteToMove>
	constexpr inline bitboard getAllPieces() const {
		if constexpr (whiteToMove) {
			return this->whitePawns | this->whiteKnights | this->whiteBishops | this->whiteRooks | this->whiteQueens | Constants::SQUARE_BBS[this->whiteKing];
		}
		else {
			return this->blackPawns | this->blackKnights | this->blackBishops | this->blackRooks | this->blackQueens | Constants::SQUARE_BBS[this->blackKing];
		}
	}

	constexpr inline bitboard getAllPieces() const {
		return this->getAllPieces<true>() | this->getAllPieces<false>();
	}

	template <bool whiteToMove>
	constexpr inline bitboard getEnemyPieces() const {
		return this->getAllPieces<!whiteToMove>();
	}

	constexpr inline bitboard getFreeSquares() const {
		return ~this->getAllPieces();
	}

	constexpr inline bool isWhitePieceAt(Index square) const {
		bitboard squareBB = Constants::SQUARE_BBS[square];
		return (this->getAllPieces<true>() & Constants::SQUARE_BBS[square]) != 0;
	}

	template <bool whiteToMove>
	constexpr inline bitboard notFriendlyPieces() const {
		return ~this->getAllPieces<whiteToMove>();
	}

	template <bool whiteToMove>
	constexpr inline Piece getPieceAt(Index square) const {
		bitboard squareBB = Constants::SQUARE_BBS[square];
		if ((this->getPawns<whiteToMove>() & squareBB) != 0) return Chess::PAWN;
		if ((this->getKnights<whiteToMove>() & squareBB) != 0) return Chess::KNIGHT;
		if ((this->getBishops<whiteToMove>() & squareBB) != 0) return Chess::BISHOP;
		if ((this->getRooks<whiteToMove>() & squareBB) != 0) return Chess::ROOK;
		if ((this->getQueens<whiteToMove>() & squareBB) != 0) return Chess::QUEEN;
		if (this->getKing<whiteToMove>() == square) return Chess::KING;
		return -1;
	}
	constexpr inline Piece getPieceAt(Index square) const {
		Piece piece = getPieceAt<true>(square);
		if (piece == -1) piece = getPieceAt<false>(square);

		return piece;
	}
	constexpr inline Piece getPieceAt(Index square, bool& color) const {
		Piece piece = getPieceAt<true>(square);
		if (piece == -1) {
			piece = getPieceAt<false>(square);
			color = false;
		}
		else color = true;

		return piece;
	}
	template <Piece piece, bool whiteToMove>
	inline void setPiece(Index index) {
		if constexpr (whiteToMove) {
			if constexpr (piece == Chess::PAWN) whitePawns |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::KNIGHT) whiteKnights |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::BISHOP) whiteBishops |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::ROOK) whiteRooks |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::QUEEN) whiteQueens |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::KING) whiteKing = index;
		}
		else {
			if constexpr (piece == Chess::PAWN) blackPawns |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::KNIGHT) blackKnights |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::BISHOP) blackBishops |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::ROOK) blackRooks |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::QUEEN) blackQueens |= Constants::SQUARE_BBS[index];
			if constexpr (piece == Chess::KING) blackKing = index;
		}
	}
	template <bool whiteToMove>
	inline void setPiece(Piece piece, Index index) {
		switch (piece) {
		case Chess::PAWN:
			this->setPiece<Chess::PAWN, whiteToMove>(index);
			break;
		case Chess::KNIGHT:
			this->setPiece<Chess::KNIGHT, whiteToMove>(index);
			break;
		case Chess::BISHOP:
			this->setPiece<Chess::BISHOP, whiteToMove>(index);
			break;
		case Chess::ROOK:
			this->setPiece<Chess::ROOK, whiteToMove>(index);
			break;
		case Chess::QUEEN:
			this->setPiece<Chess::QUEEN, whiteToMove>(index);
			break;
		case Chess::KING:
			this->setPiece<Chess::KING, whiteToMove>(index);
			break;
		default:
			throw std::invalid_argument("Board::setPiece - invalid piece type");
		}
	}

	inline void setCastlingRights(bool whiteLeft, bool whiteRight, bool blackLeft, bool blackRight) {
		this->castlingRights = (whiteLeft ? 0b1000 : 0) | (whiteRight ? 0b0100 : 0) | (blackLeft ? 0b0010 : 0) | (blackRight ? 0b0001 : 0);
	}

	template <bool whiteToMove>
	inline bool canCastleShort() const {
		if constexpr (whiteToMove) {
			return (this->castlingRights & shortCastleMask & whiteCastleMask) != 0;
		}
		else {
			return (this->castlingRights & shortCastleMask & blackCastleMask) != 0;
		}
	}

	template <bool whiteToMove>
	inline bool canCastleLong() const {
		if constexpr (whiteToMove) {
			return (this->castlingRights & longCastleMask & whiteCastleMask) != 0;
		}
		else {
			return (this->castlingRights & longCastleMask & blackCastleMask) != 0;
		}
	}
	
	template <bool whiteToMove>
	inline bool canCastle() const {
		if constexpr (whiteToMove) {
			return (this->castlingRights & whiteCastleMask) != 0;
		}
		else {
			return (this->castlingRights & blackCastleMask) != 0;
		}
	}

	inline Index getEnPassantSquare() const {
		return Chess::lsb(this->enPassant);
	}

	constexpr inline Index getEnPassantFile() const {
		if (this->enPassant == 0) return Chess::FILE_SIZE;
		return Chess::fileOf(Chess::lsb(this->enPassant));
	}

	template <bool whiteToMove, Piece piece, Flag flag = Chess::QUIET>
	Board branch(bitboard from, bitboard to) const {
		bitboard change = from | to;
		bitboard notFrom = ~from;
		bitboard notTo = ~to;
		bitboard enPassantTemp;
		uint8_t castlingRightsTemp = this->castlingRights;

		if constexpr (flag == Chess::DOUBLE_PAWN_PUSH) {
			enPassantTemp = Chess::pawnsForward<whiteToMove>(from);
		}
		else {
			enPassantTemp = 0;
		}

		// handle castling rights.
		if constexpr (flag == Chess::REMOVE_ALL_CASTLING || flag == Chess::SHORT_CASTLING || flag == Chess::LONG_CASTLING) {
			if constexpr (whiteToMove) {
				castlingRightsTemp &= ~whiteCastleMask;
			}
			else {
				castlingRightsTemp &= ~blackCastleMask;
			}
		}
		else if constexpr (flag == Chess::REMOVE_SHORT_CASTLING) {
			if constexpr (whiteToMove) {
				castlingRightsTemp &= ~(whiteCastleMask & shortCastleMask);
			}
			else {
				castlingRightsTemp &= ~(blackCastleMask & shortCastleMask);
			}
		}
		else if constexpr (flag == Chess::REMOVE_LONG_CASTLING) {
			if constexpr (whiteToMove) {
				castlingRightsTemp &= ~(whiteCastleMask & longCastleMask);
			}
			else {
				castlingRightsTemp &= ~(blackCastleMask & longCastleMask);
			}
		}
		else {
			// checking if capturing enemy rooks affect enemy castling rights.
			bitboard capture = to & Board::startingRooks<!whiteToMove>();
			if (capture == Board::startingQueensideRook<!whiteToMove>()) {
				if constexpr (!whiteToMove) {
					castlingRightsTemp &= ~whiteLongCastleMask;
				}
				else {
					castlingRightsTemp &= ~blackLongCastleMask;
				}
			}
			else if (capture == Board::startingKingsideRook<!whiteToMove>()) {
				if constexpr (!whiteToMove) {
					castlingRightsTemp &= ~whiteShortCastleMask;
				}
				else {
					castlingRightsTemp &= ~blackShortCastleMask;
				}
			}
		}

		// check promotions.
		if constexpr (flag == Chess::BISHOP_PROMOTION) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns & notFrom, this->whiteKnights, this->whiteBishops | to, this->whiteRooks,
					this->whiteQueens, this->whiteKing, this->blackPawns & notTo, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo, this->whiteKnights & notTo, this->whiteBishops & notTo, this->whiteRooks & notTo,
					this->whiteQueens & notTo, this->whiteKing, this->blackPawns & notFrom, this->blackKnights,
					this->blackBishops | to, this->blackRooks, this->blackQueens, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
		}
		else if constexpr (flag == Chess::ROOK_PROMOTION) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns & notFrom, this->whiteKnights, this->whiteBishops, this->whiteRooks | to,
					this->whiteQueens, this->whiteKing, this->blackPawns & notTo, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo, this->whiteKnights & notTo, this->whiteBishops & notTo, this->whiteRooks & notTo,
					this->whiteQueens & notTo, this->whiteKing, this->blackPawns & notFrom, this->blackKnights,
					this->blackBishops, this->blackRooks | to, this->blackQueens, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
		}
		else if constexpr (flag == Chess::QUEEN_PROMOTION) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns & notFrom, this->whiteKnights, this->whiteBishops, this->whiteRooks,
					this->whiteQueens | to, this->whiteKing, this->blackPawns & notTo, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo, this->whiteKnights & notTo, this->whiteBishops & notTo, this->whiteRooks & notTo,
					this->whiteQueens & notTo, this->whiteKing, this->blackPawns & notFrom, this->blackKnights,
					this->blackBishops, this->blackRooks, this->blackQueens | to, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
		}
		else if constexpr (flag == Chess::KNIGHT_PROMOTION) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns & notFrom, this->whiteKnights | to, this->whiteBishops, this->whiteRooks,
					this->whiteQueens, this->whiteKing, this->blackPawns & notTo, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo, this->whiteKnights & notTo, this->whiteBishops & notTo, this->whiteRooks & notTo,
					this->whiteQueens & notTo, this->whiteKing, this->blackPawns & notFrom, this->blackKnights | to,
					this->blackBishops, this->blackRooks, this->blackQueens, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
		}

		// handle castle.
		if constexpr (flag == Chess::SHORT_CASTLING) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns, this->whiteKnights, this->whiteBishops,
					this->whiteRooks ^ this->getRookShortCastlingMove<whiteToMove>(),
					this->whiteQueens, Chess::lsb(to), this->blackPawns, this->blackKnights,
					this->blackBishops, this->blackRooks, this->blackQueens, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns, this->whiteKnights, this->whiteBishops, this->whiteRooks,
					this->whiteQueens, this->whiteKing, this->blackPawns, this->blackKnights,
					this->blackBishops, this->blackRooks ^ this->getRookShortCastlingMove<whiteToMove>(),
					this->blackQueens, Chess::lsb(to), castlingRightsTemp, enPassantTemp);
			}
		}
		else if constexpr (flag == Chess::LONG_CASTLING) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns, this->whiteKnights, this->whiteBishops,
					this->whiteRooks ^ this->getRookLongCastlingMove<whiteToMove>(),
					this->whiteQueens, Chess::lsb(to), this->blackPawns, this->blackKnights,
					this->blackBishops, this->blackRooks, this->blackQueens, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns, this->whiteKnights, this->whiteBishops, this->whiteRooks,
					this->whiteQueens, this->whiteKing, this->blackPawns, this->blackKnights,
					this->blackBishops, this->blackRooks ^ this->getRookLongCastlingMove<whiteToMove>(),
					this->blackQueens, Chess::lsb(to), castlingRightsTemp, enPassantTemp);
			}
		}

		// handle normal move.
		if constexpr (piece == Chess::PAWN) {
			bitboard enPassantMask = Chess::MAX_BITBOARD;

			if constexpr (flag == Chess::EN_PASSANT) enPassantMask &= ~Chess::pawnsBackward<whiteToMove>(this->enPassant);

			if constexpr (whiteToMove) {
				return Board(this->whitePawns ^ change, this->whiteKnights, this->whiteBishops,
					this->whiteRooks, this->whiteQueens, this->whiteKing, this->blackPawns & notTo & enPassantMask, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo & enPassantMask, this->whiteKnights & notTo, this->whiteBishops & notTo,
					this->whiteRooks & notTo, this->whiteQueens & notTo, this->whiteKing, this->blackPawns ^ change, this->blackKnights,
					this->blackBishops, this->blackRooks, this->blackQueens, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
		}
		else if constexpr (piece == Chess::KNIGHT) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns, this->whiteKnights ^ change, this->whiteBishops,
					this->whiteRooks, this->whiteQueens, this->whiteKing, this->blackPawns & notTo, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo, this->whiteKnights & notTo, this->whiteBishops & notTo,
					this->whiteRooks & notTo, this->whiteQueens & notTo, this->whiteKing, this->blackPawns, this->blackKnights ^ change,
					this->blackBishops, this->blackRooks, this->blackQueens, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
		}
		else if constexpr (piece == Chess::BISHOP) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns, this->whiteKnights, this->whiteBishops ^ change,
					this->whiteRooks, this->whiteQueens, this->whiteKing, this->blackPawns & notTo, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo, this->whiteKnights & notTo, this->whiteBishops & notTo,
					this->whiteRooks & notTo, this->whiteQueens & notTo, this->whiteKing, this->blackPawns, this->blackKnights,
					this->blackBishops ^ change, this->blackRooks, this->blackQueens, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
		}
		else if constexpr (piece == Chess::ROOK) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns, this->whiteKnights, this->whiteBishops,
					this->whiteRooks ^ change, this->whiteQueens, this->whiteKing, this->blackPawns & notTo, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo, this->whiteKnights & notTo, this->whiteBishops & notTo,
					this->whiteRooks & notTo, this->whiteQueens & notTo, this->whiteKing, this->blackPawns, this->blackKnights,
					this->blackBishops, this->blackRooks ^ change, this->blackQueens, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
		}
		else if constexpr (piece == Chess::QUEEN) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns, this->whiteKnights, this->whiteBishops,
					this->whiteRooks, this->whiteQueens ^ change, this->whiteKing, this->blackPawns & notTo, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo, this->whiteKnights & notTo, this->whiteBishops & notTo,
					this->whiteRooks & notTo, this->whiteQueens & notTo, this->whiteKing, this->blackPawns, this->blackKnights,
					this->blackBishops, this->blackRooks, this->blackQueens ^ change, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
		}
		else if constexpr (piece == Chess::KING) {
			if constexpr (whiteToMove) {
				return Board(this->whitePawns, this->whiteKnights, this->whiteBishops,
					this->whiteRooks, this->whiteQueens, Chess::lsb(to), this->blackPawns & notTo, this->blackKnights & notTo,
					this->blackBishops & notTo, this->blackRooks & notTo, this->blackQueens & notTo, this->blackKing, castlingRightsTemp, enPassantTemp);
			}
			else {
				return Board(this->whitePawns & notTo, this->whiteKnights & notTo, this->whiteBishops & notTo,
					this->whiteRooks & notTo, this->whiteQueens & notTo, this->whiteKing, this->blackPawns, this->blackKnights,
					this->blackBishops, this->blackRooks, this->blackQueens, Chess::lsb(to), castlingRightsTemp, enPassantTemp);
			}
		}
	}

	template <bool whiteToMove>
	Board branch(Move move) const {
		switch (move.piece) {
			case Chess::PAWN:
			{
				switch (move.flag) {
					case Chess::QUIET:
					{
						return this->branch<whiteToMove, Chess::PAWN, Chess::QUIET>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::DOUBLE_PAWN_PUSH:
					{
						return this->branch<whiteToMove, Chess::PAWN, Chess::DOUBLE_PAWN_PUSH>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::EN_PASSANT:
					{
						return this->branch<whiteToMove, Chess::PAWN, Chess::EN_PASSANT>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::KNIGHT_PROMOTION:
					{
						return this->branch<whiteToMove, Chess::PAWN, Chess::KNIGHT_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::BISHOP_PROMOTION:
					{
						return this->branch<whiteToMove, Chess::PAWN, Chess::BISHOP_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::ROOK_PROMOTION:
					{
						return this->branch<whiteToMove, Chess::PAWN, Chess::ROOK_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::QUEEN_PROMOTION:
					{
						return this->branch<whiteToMove, Chess::PAWN, Chess::QUEEN_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
				}
				return *this;
			}
			case Chess::KNIGHT:
			{
				return this->branch<whiteToMove, Chess::KNIGHT>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
			}
			case Chess::BISHOP:
			{
				return this->branch<whiteToMove, Chess::BISHOP>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
			}
			case Chess::ROOK:
			{
				switch (move.flag) {
					case Chess::QUIET:
					{
						return this->branch<whiteToMove, Chess::ROOK, Chess::QUIET>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::REMOVE_SHORT_CASTLING:
					{
						return this->branch<whiteToMove, Chess::ROOK, Chess::REMOVE_SHORT_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::REMOVE_LONG_CASTLING:
					{
						return this->branch<whiteToMove, Chess::ROOK, Chess::REMOVE_LONG_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
				}
			}
			case Chess::QUEEN:
			{
				return this->branch<whiteToMove, Chess::QUEEN>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
			}
			case Chess::KING:
			{
				switch (move.flag) {
					case Chess::REMOVE_ALL_CASTLING:
					{
						return this->branch<whiteToMove, Chess::KING, Chess::REMOVE_ALL_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::SHORT_CASTLING:
					{
						return this->branch<whiteToMove, Chess::KING, Chess::SHORT_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
					case Chess::LONG_CASTLING:
					{
						return this->branch<whiteToMove, Chess::KING, Chess::LONG_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
					}
				}
			}
		}
		return *this;
	}

	template <bool whiteToMove, Piece piece, Flag flag = Chess::QUIET>
	void makeMove(bitboard from, bitboard to) {
		bitboard change = from | to;
		bitboard notFrom = ~from;
		bitboard notTo = ~to;
		bitboard enPassantTemp = this->enPassant;

		// handle pawn moved 2 squares.
		if constexpr (flag == Chess::DOUBLE_PAWN_PUSH) {
			this->enPassant = Chess::pawnsForward<whiteToMove>(from);
		}
		else {
			this->enPassant = 0;
		}

		// check promotions.
		if constexpr (flag == Chess::BISHOP_PROMOTION || flag == Chess::ROOK_PROMOTION || flag == Chess::KNIGHT_PROMOTION || flag == Chess::QUEEN_PROMOTION) {
			if constexpr (whiteToMove) {
				this->blackPawns &= notTo;
				this->blackKnights &= notTo;
				this->blackRooks &= notTo;
				this->blackQueens &= notTo;

				this->whitePawns &= notFrom;
			}
			else {
				this->whitePawns &= notTo;
				this->whiteKnights &= notTo;
				this->whiteRooks &= notTo;
				this->whiteQueens &= notTo;

				this->blackPawns &= notFrom;
			}

			if constexpr (flag == Chess::BISHOP_PROMOTION) {
				if constexpr (whiteToMove) {
					this->whiteBishops |= to;
				}
				else {
					this->blackBishops |= to;
				}
			}
			else if constexpr (flag == Chess::ROOK_PROMOTION) {
				if constexpr (whiteToMove) {
					this->whiteRooks |= to;
				}
				else {
					this->blackRooks |= to;
				}
			}
			else if constexpr (flag == Chess::QUEEN_PROMOTION) {
				if constexpr (whiteToMove) {
					this->whiteQueens |= to;
				}
				else {
					this->blackQueens |= to;
				}
			}
			else if constexpr (flag == Chess::KNIGHT_PROMOTION) {
				if constexpr (whiteToMove) {
					this->whiteKnights |= to;
				}
				else {
					this->blackKnights |= to;
				}
			}

			return;
		}

		// handle castling rights.
		if constexpr (flag == Chess::REMOVE_ALL_CASTLING || flag == Chess::SHORT_CASTLING || flag == Chess::LONG_CASTLING) {
			if constexpr (whiteToMove) {
				this->castlingRights &= ~whiteCastleMask;
			}
			else {
				this->castlingRights &= ~blackCastleMask;
			}
		}
		else if constexpr (flag == Chess::REMOVE_SHORT_CASTLING) {
			if constexpr (whiteToMove) {
				this->castlingRights &= ~(whiteCastleMask & shortCastleMask);
			}
			else {
				this->castlingRights &= ~(blackCastleMask & shortCastleMask);
			}
		}
		else if constexpr (flag == Chess::REMOVE_LONG_CASTLING) {
			if constexpr (whiteToMove) {
				this->castlingRights &= ~(whiteCastleMask & longCastleMask);
			}
			else {
				this->castlingRights &= ~(blackCastleMask & longCastleMask);
			}
		}

		// checking if capturing enemy rooks affect enemy castling rights.
		bitboard capture = to & Board::startingRooks<!whiteToMove>();
		if (capture == Board::startingQueensideRook<!whiteToMove>()) {
				if constexpr (whiteToMove) {
					this->castlingRights &= ~whiteLongCastleMask;
				}
				else {
					this->castlingRights &= ~blackLongCastleMask;
				}
			}
		else if (capture == Board::startingKingsideRook<!whiteToMove>()) {
			if constexpr (whiteToMove) {
				this->castlingRights &= ~whiteShortCastleMask;
			}
			else {
				this->castlingRights &= ~blackShortCastleMask;
			}
		}

		// handle castle.
		if constexpr (flag == Chess::SHORT_CASTLING) {
			if constexpr (whiteToMove) {
				this->whiteRooks ^= this->getRookShortCastlingMove<whiteToMove>();
				this->whiteKing = Chess::lsb(to);
			}
			else {
				this->blackRooks ^= this->getRookShortCastlingMove<whiteToMove>();
				this->blackKing = Chess::lsb(to);
			}

			return;
		}
		else if constexpr (flag == Chess::LONG_CASTLING) {
			if constexpr (whiteToMove) {
				this->whiteRooks ^= this->getRookLongCastlingMove<whiteToMove>();
				this->whiteKing = Chess::lsb(to);
			}
			else {
				this->blackRooks ^= this->getRookLongCastlingMove<whiteToMove>();
				this->blackKing = Chess::lsb(to);
			}

			return;
		}

		// remove possible captures.
		if constexpr (whiteToMove) {
			this->blackPawns &= notTo;
			this->blackKnights &= notTo;
			this->blackBishops &= notTo;
			this->blackRooks &= notTo;
			this->blackQueens &= notTo;
		}
		else {
			this->whitePawns &= notTo;
			this->whiteKnights &= notTo;
			this->whiteBishops &= notTo;
			this->whiteRooks &= notTo;
			this->whiteQueens &= notTo;
		}

		// handle normal move.
		if constexpr (piece == Chess::PAWN) {
			bitboard enPassantMask = Chess::MAX_BITBOARD;

			if constexpr (flag == Chess::EN_PASSANT) enPassantMask &= ~enPassantTemp;

			if constexpr (whiteToMove) {
				this->whitePawns ^= change;
				this->blackPawns &= enPassantMask;
			}
			else {
				this->blackPawns ^= change;
				this->whitePawns &= enPassantMask;
			}
		}
		else if constexpr (piece == Chess::KNIGHT) {
			if constexpr (whiteToMove) {
				this->whiteKnights ^= change;
			}
			else {
				this->blackKnights ^= change;
			}
		}
		else if constexpr (piece == Chess::BISHOP) {
			if constexpr (whiteToMove) {
				this->whiteBishops ^= change;
			}
			else {
				this->blackBishops ^= change;
			}
		}
		else if constexpr (piece == Chess::ROOK) {
			if constexpr (whiteToMove) {
				this->whiteRooks ^= change;
			}
			else {
				this->blackRooks ^= change;
			}
		}
		else if constexpr (piece == Chess::QUEEN) {
			if constexpr (whiteToMove) {
				this->whiteQueens ^= change;
			}
			else {
				this->blackQueens ^= change;
			}
		}
		else if constexpr (piece == Chess::KING) {
			if constexpr (whiteToMove) {
				this->whiteKing = Chess::lsb(to);
			}
			else {
				this->blackKing = Chess::lsb(to);
			}
		}
	}

	template <bool whiteToMove>
	void makeMove(Move move) {
		switch (move.piece) {
			case Chess::PAWN:
			{
				switch (move.flag) {
					case Chess::QUIET:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::QUIET>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::DOUBLE_PAWN_PUSH:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::DOUBLE_PAWN_PUSH>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::EN_PASSANT:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::EN_PASSANT>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::KNIGHT_PROMOTION:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::KNIGHT_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::BISHOP_PROMOTION:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::BISHOP_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::ROOK_PROMOTION:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::ROOK_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::QUEEN_PROMOTION:
					{
						this->makeMove<whiteToMove, Chess::PAWN, Chess::QUEEN_PROMOTION>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
				}
				return;
			}
			case Chess::KNIGHT:
			{
				this->makeMove<whiteToMove, Chess::KNIGHT>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
				return;
			}
			case Chess::BISHOP:
			{
				this->makeMove<whiteToMove, Chess::BISHOP>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
				return;
			}
			case Chess::ROOK:
			{
				switch (move.flag) {
					case Chess::QUIET:
					{
						this->makeMove<whiteToMove, Chess::ROOK, Chess::QUIET>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::REMOVE_SHORT_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::ROOK, Chess::REMOVE_SHORT_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::REMOVE_LONG_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::ROOK, Chess::REMOVE_LONG_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
				}
				return;
			}
			case Chess::QUEEN:
			{
				this->makeMove<whiteToMove, Chess::QUEEN>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
				return;
			}
			case Chess::KING:
			{
				switch (move.flag) {
					case Chess::REMOVE_ALL_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::KING, Chess::REMOVE_ALL_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::SHORT_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::KING, Chess::SHORT_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
					case Chess::LONG_CASTLING:
					{
						this->makeMove<whiteToMove, Chess::KING, Chess::LONG_CASTLING>(Constants::SQUARE_BBS[move.from], Constants::SQUARE_BBS[move.to]);
						return;
					}
				}
			}
		}
	}

	void makeMove(Move move, bool whiteToMove) {
		if (whiteToMove) {
			this->makeMove<true>(move);
		}
		else {
			this->makeMove<false>(move);
		}
	}

	template <bool isWhite>
	inline bool hasInsufficientMaterial() {
		if (this->getPawns<isWhite>() == 0 && this->getQueens<isWhite>() == 0 &&
			this->getRooks<isWhite>() == 0) {
			bitboard bishopsAndKnights = this->getBishops<isWhite>() | this->getKnights<isWhite>();
			Index bitsNum = Chess::numOfBits(bishopsAndKnights);
			if (bitsNum < 2) {
				return true;
			}
			else if (bitsNum == 2) {
				if (bishopsAndKnights == this->getKnights<isWhite>()) return true;
				if (bishopsAndKnights == this->getBishops<isWhite>()) {
					Index firstBishopSquare = Chess::popLSB(bishopsAndKnights);
					Index secondBishopSquare = Chess::popLSB(bishopsAndKnights);
					if ((Chess::fileOf(firstBishopSquare) + Chess::rankOf(firstBishopSquare)) % 2 ==
						(Chess::fileOf(secondBishopSquare) + Chess::rankOf(secondBishopSquare)) % 2) {
						return true;
					}
				}
			}
			else {
				if (bishopsAndKnights == this->getBishops<isWhite>()) {
					Index firstBishopSquare = Chess::popLSB(bishopsAndKnights);
					bool color = (Chess::fileOf(firstBishopSquare) + Chess::rankOf(firstBishopSquare)) % 2;
					while (bishopsAndKnights) {
						Index bishopSquare = Chess::popLSB(bishopsAndKnights);
						if ((Chess::fileOf(bishopSquare) + Chess::rankOf(bishopSquare)) % 2 != color) {
							return false;
						}
					}
					return true;
				}
			}
		}

		return false;
	}
};

#endif
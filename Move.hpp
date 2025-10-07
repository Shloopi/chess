#ifndef MOVE_HPP
#define MOVE_HPP

#include <cstdint>
#include "Chess.hpp"
#include "Board.hpp"

enum class MoveType : uint8_t {
    Quiet,
    Capture,
    DoublePawnPush,
    KingCastle,
    QueenCastle,
    EnPassant,
    Promotion,
    PromotionCapture
};

struct Move {
    Index from;
    Index to;
    Piece piece;
    MoveType type;
    Piece capture;
    Piece promote;

	Move() : from(-1), to(-1), piece(Piece::NONE), type(MoveType::Quiet), capture(Piece::NONE), promote(Piece::NONE) {}
	Move(Index f, Index t, Piece p, MoveType mt, Piece c = Piece::NONE, Piece pr = Piece::NONE) : from(f), to(t), piece(p), type(mt), capture(c), promote(pr) {}

    friend std::ostream& operator<<(std::ostream& os, const Move& move) {
        os << Square::getNotation(move.from) << " -> " << Square::getNotation(move.to) << " - " << PieceHelper::getName(move.piece);

		return os;
    }

	inline bool isCapture() const { return type == MoveType::Capture || type == MoveType::PromotionCapture || type == MoveType::EnPassant; }
	inline bool isPromotion() const { return type == MoveType::Promotion || type == MoveType::PromotionCapture; }
	inline bool isCastling() const { return type == MoveType::KingCastle || type == MoveType::QueenCastle; }
};



#endif
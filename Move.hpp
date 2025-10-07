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
    ColoredPiece capture;
    ColoredPiece promote;

	Move() : from(-1), to(-1), piece(Piece::NONE), type(MoveType::Quiet), capture(ColoredPiece::COLORED_NONE), promote(ColoredPiece::COLORED_NONE) {}
	Move(Index f, Index t, Piece p, MoveType mt, ColoredPiece c = ColoredPiece::COLORED_NONE, ColoredPiece pr = ColoredPiece::COLORED_NONE) : from(f), to(t), piece(p), type(mt), capture(c), promote(pr) {}

    friend std::ostream& operator<<(std::ostream& os, const Move& move) {
        os << Square::getNotation(move.from) << Square::getNotation(move.to);
        if (move.type == MoveType::Promotion || move.type == MoveType::PromotionCapture) {
            os << PieceHelper::getChar(move.promote);
        }
		return os;
    }

};



#endif
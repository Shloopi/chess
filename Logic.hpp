#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "Board.hpp"
#include "MoveGen.hpp"

namespace Logic {
    void calcPins(const Board& board) {
        Index kingSquare = board.getKing();

        // get pseudo moves from the king's square.
        bitboard bishopMoves = MoveGen::getPseudoBishopMoves(*this, kingSquare, this->allPieces);
        bitboard rookMoves = MoveGen::getPseudoRookMoves(*this, kingSquare, this->allPieces);
        bitboard queenMoves = MoveGen::getPseudoQueenMoves(*this, kingSquare, this->allPieces);

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
    bitboard Board::processPins(const Board& board, bitboard possiblePinningPieces, bitboard kingRayMoves) const {
        Index square;
        bitboard pinningPieceMoves, pins = 0ULL;

        // check for each potential pinning piece.
        while (possiblePinningPieces != 0) {
            square = Chess::lsb(possiblePinningPieces);
            possiblePinningPieces &= (possiblePinningPieces - 1);

            // get the pseudo moves of that piece.
            if constexpr (forBishopPins)
                pinningPieceMoves = MoveGen::getPseudoBishopMoves(*this, square, this->allPieces);
            else
                pinningPieceMoves = MoveGen::getPseudoRookMoves(*this, square, this->allPieces);

            // if the moves of the piece and the moves from the king overlapps, there is a pinned piece in between.
            pins |= (kingRayMoves & pinningPieceMoves);
        }

        return pins;
    }
    bitboard calcChecks(const Board& board) {
        Index kingSquare = board.getKingPos();

        bitboard checks = 0ULL;

        // knight checks.
        checks |= (Constants::KNIGHT_MOVES[kingSquare] & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_KNIGHT : ColoredPiece::WHITE_KNIGHT]);

        // bishop checks.
        checks |= (MoveGen::getPseudoBishopMoves(board, kingSquare, this->allPieces) & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_BISHOP : ColoredPiece::WHITE_BISHOP]);

        // rook checks.
        checks |= (MoveGen::getPseudoRookMoves(board, kingSquare, this->allPieces) & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_ROOK : ColoredPiece::WHITE_ROOK]);

        // queen checks.
        checks |= (MoveGen::getPseudoQueenMoves(board, kingSquare, this->allPieces) & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_QUEEN : ColoredPiece::WHITE_QUEEN]);

        // pawn checks.
        checks |= MoveGen::getPawnCaptures(this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_PAWN : ColoredPiece::WHITE_PAWN], !this->info.whiteMove) & Constants::SQUARE_BBS[kingSquare];
        
        return checks;
    }
    bool isSquareAttacked(Index square, bitboard pieces) const {
        // knight attacks.
        if ((Constants::KNIGHT_MOVES[square] & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_KNIGHT : ColoredPiece::WHITE_KNIGHT]) != 0) return true;

        // bishop attacks.
        if ((MoveGen::getPseudoBishopMoves(*this, square, pieces) & this->getPieces(Piece::BISHOP, !this->whiteToMove())) != 0) return true;

        // rook attacks.
        if ((MoveGen::getPseudoRookMoves(*this, square, pieces) & this->getPieces(Piece::ROOK, !this->whiteToMove())) != 0) return true;

        // queen attacks.
        if ((MoveGen::getPseudoQueenMoves(*this, square, pieces) & this->getPieces(Piece::QUEEN, !this->whiteToMove())) != 0) return true;

        // pawn attacks.
        if ((MoveGen::getPawnCaptures(this->getPieces(Piece::PAWN, !this->whiteToMove()), !this->info.whiteMove) & Constants::SQUARE_BBS[square]) != 0) return true;

        return false;
    }
}

#endif
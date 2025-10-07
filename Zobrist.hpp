#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <random>
#include "Bitboard.hpp"
#include "BoardInfo.hpp"
#include "Piece.hpp"

namespace Zobrist {
    inline std::array<std::array<bitboard, 64>, 12> piecesRandom;
    inline std::array<bitboard, 2> turnRandom;
    inline std::array<bitboard, 16> castlingRandom;
    inline std::array<bitboard, 9> enPassantFileRandom;
    inline bitboard seed = 0x5523FEDC480CE0AF;

    void init();
    bitboard genKey(bool whiteToMove);
    bitboard applyPiece(bitboard key, Piece piece, Index square);
    bitboard applyTurn(bitboard key, bool whiteToMove);
    bitboard applyBoardInfo(bitboard key, const BoardInfo& info);


    bitboard genRandomNumber();




};


#endif
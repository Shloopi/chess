#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <random>
#include "Chess.hpp"
#include <array>

namespace Zobrist {
    inline std::array<std::array<std::array<uint64_t, 64>, 6>, 2> piecesRandom;
    inline std::array<uint64_t, 2> turnRandom;
    inline std::array<uint64_t, 16> castlingRandom;
    inline std::array<uint64_t, 9> enPassantFileRandom;
    inline uint64_t seed = 0x5523FEDC480CE0AF;

    void init();
    uint64_t genKey(bool whiteToMove);

    uint64_t applyPiece(uint64_t key, bool whiteToMove, Piece piece, Index square);
    uint64_t applyTurn(uint64_t key, bool whiteToMove);
    uint64_t applyBoard(uint64_t key, uint8_t castlingRights, Index enPassantFile);


    bitboard genRandomNumber();




};


#endif
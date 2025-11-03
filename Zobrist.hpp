#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <random>
#include "Chess.hpp"
#include "Board.hpp"
#include <array>

namespace Zobrist {
    inline std::array<std::array<std::array<uint64_t, 64>, 6>, 2> piecesRandom;
    inline std::array<uint64_t, 2> turnRandom;
    inline std::array<uint64_t, 16> castlingRandom;
    inline std::array<uint64_t, 9> enPassantFileRandom;
    inline uint64_t seed = 0x5523FEDC480CE0AF;

	template <bool whiteToMove>
    inline uint64_t hash(const Board& board);

    void init();
    uint64_t genRandomNumber();




};


#endif
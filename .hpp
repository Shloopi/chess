#ifndef PRIOR_GEN_HPP
#define PRIOR_GEN_HPP

#include <array>
#include <vector>
#include "Chess.hpp"
#include "Bitboard.hpp"

class PriorGen {
private:
    inline static std::array<bitboard, Chess::BOARD_SIZE> knightMoves;
    inline static std::array<bitboard, Chess::BOARD_SIZE> kingMoves;
    inline static std::array<bitboard, Chess::BOARD_SIZE> diagBasicMoves;
    inline static std::array<bitboard, Chess::BOARD_SIZE> orthoBasicMovesForMagic;
    inline static std::array<bitboard, Chess::BOARD_SIZE> orthoBasicMoves;

    inline static std::array<std::vector<bitboard>, Chess::BOARD_SIZE> diagLookupTable;
    inline static std::array <std::vector<bitboard>, Chess::BOARD_SIZE> orthoLookupTable;
    inline static std::array<std::array<bitboard, Chess::BOARD_SIZE>, Chess::BOARD_SIZE> betweenTable;
    inline static std::array<std::array<bitboard, Chess::BOARD_SIZE>, Chess::BOARD_SIZE> betweenPiecesTable;

    static void genKnightMoves();
    static void genKingMoves();
    static void genDiagBasicMoves(const std::array<Square, 4>& directions);
    static void genOrthoBasicMoves();
    static void genLookupTable(bool isDiag, const std::array<Square, 4>& directions);
    static void genBetweenTable();
    static void genBetweenPiecesTable();
    static bitboard genSlidingAttack(Square sourceSquare, bitboard occupancy, const std::array<Square, 4>& directions);
    static std::vector<bitboard> genOccupancies(bitboard moves);
    static inline bitboard genMagicIndex(bitboard bb, bitboard magicNumber, Index shifter) { return (bb * magicNumber) >> shifter; }

    PriorGen() = delete;
    ~PriorGen() = delete;

public:
    static void Init();
};


#endif

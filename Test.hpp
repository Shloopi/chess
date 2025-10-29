#ifndef TEST_GEN_HPP
#define TEST_GEN_HPP

#include "BoardState.hpp"
#include <chrono>

namespace test {
    static inline std::array<std::array<Move, 218>, 12> movesBuffer;

    template<bool whiteToMove>
    uint64_t countMoves(BoardState& state, uint8_t depth) {
        std::array<Move, 218>& moves = movesBuffer[depth];
        unsigned short moveCount = MoveGen::genAllLegalMoves<whiteToMove>(state, &moves[0]);
 
        if (depth == 1) return moveCount;
        if (depth == 2) std::cout << "Moves: " + static_cast<int>(moveCount);
        uint64_t count = 0;

        for (unsigned short i = 0; i < moveCount; i++) {
            const Move move = moves[i];

            Board board = state.board.branch<whiteToMove>(move);
            BoardState state2 = state.branchState<!whiteToMove>(board);

            count += test::countMoves<!whiteToMove>(state2, depth - 1);
        }
        return count;
    }

    template <bool whiteToMove>
    uint64_t timeDepth(BoardState& state, uint8_t depth = 5) {
        uint64_t moves_count;

        auto start = std::chrono::high_resolution_clock::now();

        moves_count = test::countMoves<whiteToMove>(state, depth);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << static_cast<short>(depth) << ") " << moves_count << " Moves - Function took " << elapsed.count() << " ms";
        std::cout << " - " << (elapsed.count() / moves_count) << "ms Per Move." << '\n';
        return moves_count;
    }
}


#endif
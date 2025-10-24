#include "Test.hpp"
#include <chrono>
#include "MoveGen.hpp"
#include "BoardState.hpp"

template uint64_t test::timeDepth<true>(BoardState&, uint8_t);
template uint64_t test::timeDepth<false>(BoardState&, uint8_t);

template <bool whiteToMove>
uint64_t test::timeDepth(BoardState& state, uint8_t depth)
{
    bitboard moves_count;

    auto start = std::chrono::high_resolution_clock::now();

    moves_count = test::countMoves<whiteToMove>(state, depth);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Function took " << elapsed.count() << " ms\n";

    return moves_count;
}

template uint64_t test::countMoves<true>(BoardState&, uint8_t);
template uint64_t test::countMoves<false>(BoardState&, uint8_t);

template <bool whiteToMove>
uint64_t test::countMoves(BoardState& state, uint8_t depth) {
    std::array<Move, 218> moves;
    unsigned short moveCount = MoveGen::genAllLegalMoves<whiteToMove>(state, &moves[0]);

    if (depth == 1) return moveCount;

    bitboard count = 0;

    for (unsigned short i = 0; i < moveCount; i++) {

        const Move move = moves[i];

        state.board.makeMove<whiteToMove>(move);

        if constexpr (whiteToMove) {
            count += test::countMoves<false>(state, depth - 1);
        }
        else {
            count += test::countMoves<true>(state, depth - 1);

        }

        // TODO: unmake move.

    }

    return count;
}
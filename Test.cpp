#include "Test.hpp"
#include <chrono>
#include "MoveGen.hpp"
#include "Board.hpp"

uint64_t test::timeDepth(Board& board, unsigned char depth)
{
    bitboard moves_count;

    auto start = std::chrono::high_resolution_clock::now();

    moves_count = test::countMoves(board, depth);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Function took " << elapsed.count() << " ms\n";

    return moves_count;
}

uint64_t test::countMoves(Board& board, unsigned char depth) {
    std::array<Move, 218> moves;
    unsigned short moveCount = MoveGen::genAllLegalMoves(board, &moves[0]);

    if (depth == 1) return moveCount;

    bitboard count = 0;

    for (unsigned short i = 0; i < moveCount; i++) {

        const Move move = moves[i];

        board.makeMove(move);

        //if (!board.gameOver())
            count += test::countMoves(board, depth - 1);

        board.unmakeMove();

    }

    return count;
}
#ifndef TEST_GEN_HPP
#define TEST_GEN_HPP

#include "Game.hpp"
#include <chrono>
#include "MoveGen.hpp"

namespace test {
    static inline std::array<std::array<Move, 218>, 12> movesBuffer;
    inline std::unordered_map<Move, uint8_t> map;

    template<bool whiteToMove>
    uint64_t countMoves(Game& game, uint8_t depth) {
        if (game.gameState != GameState::ONGOING) {
            return 0;
		}

        std::array<Move, 218>& moves = movesBuffer[depth];
        uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(game, &moves[0]);
         
        if (depth == 1) {
            return moveCount;
        }
        
        uint64_t count = 0;

        for (uint8_t i = 0; i < moveCount; i++) {
            const Move move = moves[i];

			GameSnapshot snapshot = game.createSnapshot();
			game.makeMove<whiteToMove, true>(move);
            count += test::countMoves<!whiteToMove>(game, depth - 1);
			game.undoMove<whiteToMove>(snapshot);
        }

        
        return count;
    }

    template <bool whiteToMove>
    uint64_t timeDepth(Game& game, uint8_t depth = 5, bool print = true) {
        uint64_t moves_count;

        auto start = std::chrono::high_resolution_clock::now();

        moves_count = test::countMoves<whiteToMove>(game, depth);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        if (print) {
            if (elapsed.count() > 600'000) {
                std::cout << static_cast<short>(depth) << ") " << moves_count << " Moves - Function took " << (elapsed.count() / 60'000) << " Minutes";
            }
            else if (elapsed.count() > 10'000) {
                std::cout << static_cast<short>(depth) << ") " << moves_count << " Moves - Function took " << (elapsed.count() / 1'000) << " Seconds";
            }
            else {
                std::cout << static_cast<short>(depth) << ") " << moves_count << " Moves - Function took " << elapsed.count() << " Milli Seconds";
            }
            std::cout << " - " << (1000 * elapsed.count() / moves_count) << " Micro Seconds Per Move." << '\n';

        }
        
        //for (const auto& [key, value] : map) {
        //    std::cout << key << " -> " << (int)value << '\n';  // access key only
        //}

        return moves_count;
    }
}


#endif
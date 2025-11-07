#ifndef TEST_GEN_HPP
#define TEST_GEN_HPP

#include "../Core/Game.hpp"
#include "../Utils/Fen.hpp"
#include <chrono>

namespace Test {
    static inline std::array<std::array<Move, 218>, 12> movesBuffer;
    inline std::unordered_map<Move, uint8_t> map;

    template<bool whiteToMove>
    uint64_t perft(Game& game, uint8_t depth) {
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
            count += Test::perft<!whiteToMove>(game, depth - 1);
			game.undoMove<whiteToMove>(snapshot);
        }

        
        return count;
    }

    template <bool whiteToMove>
    uint64_t timedPerft(Game& game, uint8_t depth = 5, bool print = true) {
        uint64_t moves_count;

        auto start = std::chrono::high_resolution_clock::now();

        moves_count = Test::perft<whiteToMove>(game, depth);

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
        return moves_count;
    }

    template <bool whiteToMove>
    void loopedTimedPerft(Game& game, int from, int to) {
        for (uint8_t i = from; i <= to; i++) {
            Test::timedPerft<whiteToMove>(game, i);
        }
    }

    template <bool whiteToMove>
    void perftPerMove(Game& game, int depth) {
        std::array<Move, 218>& moves = movesBuffer[depth];
        uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(game, &moves[0]);

        for (int i = 0; i < moveCount; i++) {
            const Move move = moves[i];
            GameSnapshot snapshot = game.createSnapshot();
            game.makeMove<whiteToMove, true>(move);
            std::cout << move << " - " << Test::timedPerft<!whiteToMove>(game, depth, false) << '\n';
            game.undoMove<whiteToMove>(snapshot);
        }
    }

    template <bool whiteToMove, bool print = true>
    bool makeMove(Game& game, Index startSquare, Index targetSquare) {
        std::array<Move, 218> moves;
        uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(game, &moves[0]);

        for (int i = 0; i < moveCount; i++) {
            const Move move = moves[i];

            if (move.from == startSquare && move.to == targetSquare) {
                if constexpr (print) std::cout << "making move - " << move << '\n';
                game.makeMove<whiteToMove>(move);
                return true;
            }
        }

        return false;
    }

    template <bool whiteToMove>
    void showMoves(Game& game) {
        std::array<Move, 218>& moves = movesBuffer[0];
        uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(game, &moves[0]);

        std::cout << "Moves: " << (int)moveCount << '\n';

        for (int i = 0; i < moveCount; i++) {
            const Move move = moves[i];
            std::cout << move << '\n';
        }
    }

    template <bool whiteToMove>
    void showAfterMoveFens(Game& game) {
        std::array<Move, 218>& moves = movesBuffer[0];
        uint8_t moveCount = MoveGen::genAllLegalMoves<whiteToMove>(game, &moves[0]);

        std::cout << '{';
        for (int i = 0; i < moveCount; i++) {
            GameSnapshot snapshot = game.createSnapshot();
            game.makeMove<whiteToMove>(moves[i]);

            std::string a = Fen::genFen<!whiteToMove>(game);
            std::cout << '"' << a << '"';
            if (i + 1 < moveCount) std::cout << ", ";
            game.undoMove<whiteToMove>(snapshot);
        }
        std::cout << '}' << ';';
    }
}


#endif
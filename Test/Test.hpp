#ifndef TEST_GEN_HPP
#define TEST_GEN_HPP

#include "../Core/Game.hpp"
#include "../Utils/Fen.hpp"
#include <chrono>

namespace Test {
    static inline std::array<std::array<Move, 218>, 12> movesBuffer;
    inline std::unordered_map<Move, uint8_t> map;

    uint64_t perft(Game& game, uint8_t depth) {
        std::array<Move, 218>& moves = movesBuffer[depth];
        uint8_t moveCount = MoveGen::genAllLegalMoves(game, &moves[0]);
         
        if (depth == 1) {
            return moveCount;
        }
        
        uint64_t count = 0;

        for (uint8_t i = 0; i < moveCount; i++) {
            const Move move = moves[i];

			GameSnapshot snapshot = game.createSnapshot();
			game.makeMove<true>(move);
            count += Test::perft(game, depth - 1);
			game.undoMove(snapshot);
        }

        
        return count;
    }

    uint64_t timedPerft(Game& game, uint8_t depth = 5, bool print = true) {
        uint64_t moves_count;

        auto start = std::chrono::high_resolution_clock::now();

        moves_count = Test::perft(game, depth);

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

    void loopedTimedPerft(Game& game, int from, int to) {
        for (uint8_t i = from; i <= to; i++) {
            Test::timedPerft(game, i);
        }
    }

    void perftPerMove(Game& game, int depth) {
        std::array<Move, 218>& moves = movesBuffer[depth];
        uint8_t moveCount = MoveGen::genAllLegalMoves(game, &moves[0]);

        for (int i = 0; i < moveCount; i++) {
            const Move move = moves[i];
            GameSnapshot snapshot = game.createSnapshot();
            game.makeMove<true>(move);
            std::cout << move << " - " << Test::timedPerft(game, depth, false) << '\n';
            game.undoMove(snapshot);
        }
    }

    template <bool print = true>
    bool makeMove(Game& game, Index startSquare, Index targetSquare) {
        std::array<Move, 218> moves;
        uint8_t moveCount = MoveGen::genAllLegalMoves(game, &moves[0]);

        for (int i = 0; i < moveCount; i++) {
            const Move move = moves[i];

            if (move.from == startSquare && move.to == targetSquare) {
                if constexpr (print) std::cout << "making move - " << move << '\n';
                game.makeMove(move);
                return true;
            }
        }

        return false;
    }

    void showMoves(Game& game) {
        std::array<Move, 218>& moves = movesBuffer[0];
        uint8_t moveCount = MoveGen::genAllLegalMoves(game, &moves[0]);

        std::cout << "Moves: " << (int)moveCount << '\n';

        for (int i = 0; i < moveCount; i++) {
            const Move move = moves[i];
            std::cout << move << '\n';
        }
    }

    void showAfterMoveFens(Game& game) {
        std::array<Move, 218>& moves = movesBuffer[0];
        uint8_t moveCount = MoveGen::genAllLegalMoves(game, &moves[0]);

        std::cout << '{';
        for (int i = 0; i < moveCount; i++) {
            GameSnapshot snapshot = game.createSnapshot();
            game.makeMove(moves[i]);

            std::string a = Fen::genFen(game);
            std::cout << '"' << a << '"';
            if (i + 1 < moveCount) std::cout << ", ";
            game.undoMove(snapshot);
        }
        std::cout << '}' << ';';
    }
}


#endif
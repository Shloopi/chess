#ifndef BOT_CONST_HPP
#define BOT_CONST_HPP

#include "../Core/Chess.hpp"

constexpr int MAX_SCORE = 99999;
constexpr int MIN_SCORE = -99999;

struct EvalMove {
    Move move;
    int score;

    EvalMove() : score(MIN_SCORE) {}
    EvalMove(const Move& m) : move(m), score(MIN_SCORE) {}
    EvalMove(const Move& m, int s) : move(m), score(s) {}

    EvalMove(const EvalMove&) = default;
    EvalMove& operator=(const Move& m) {
        move = m;
        score = MIN_SCORE;
        return *this;
    }
};




#endif
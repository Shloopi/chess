#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../Core/Chess.hpp"
#include "BotConst.hpp"
#include "../Core/MoveGen.hpp"

class Game;

enum class Type {
    PLAYER,
    HUMAN,
    BOT
};

class Player {
protected:
    bool isWhite;
public:
    Player(bool isWhite) : isWhite(isWhite) {}
    virtual ~Player() = default;

    bool getIsWhite() const { return isWhite; }

    virtual Type getType() const { return Type::PLAYER; }

    virtual Move getBestMove(Game& game, Moves<Move>& moves) = 0;
};

// -----------------------------

class Human : public Player {
public:
    explicit Human(bool isWhite) : Player(isWhite) {}

    Type getType() const override { return Type::HUMAN; }

    Move getBestMove(Game& game, Moves<Move>& moves) override {
        // For Human, you can return a dummy or handle SDL input
        return Move();
    }
};

// -----------------------------

class Bot : public Player {
private:
public:
    explicit Bot(bool isWhite) : Player(isWhite) {}
    Type getType() const override { return Type::BOT; }

    Move getBestMove(Game& game, Moves<Move>& moves) override;
};

#endif

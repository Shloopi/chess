#ifndef GAME_HPP
#define GAME_HPP


#include <cstdint>
#include "../Utils/Table.hpp"
#include "Board.hpp"
#include "BoardState.hpp"
#include "../Utils/Zobrist.hpp"
#include "MoveGen.hpp"
#include "../Bot/Player.hpp"

enum class GameState : uint8_t {
	ONGOING,
	CHECKMATE,
	STALEMATE,
	DRAW_BY_INSUFFICIENT_MATERIAL,
	DRAW_BY_FIFTY_MOVE_RULE,
	DRAW_BY_THREEFOLD_REPETITION
};

struct GameSnapshot {
public:
	uint8_t halfmoves;
	uint8_t fullmoves;
	std::unique_ptr<Board> boardPtr;
	uint64_t boardHash;
	GameState gameState;

	GameSnapshot(uint8_t halfmoves, uint8_t fullmoves, const Board& board, uint64_t boardHash, GameState gameState) :
		halfmoves(halfmoves), fullmoves(fullmoves), boardPtr(std::make_unique<Board>(board)), boardHash(boardHash), gameState(gameState)
	{
	}

	GameSnapshot(GameSnapshot&&) noexcept = default;
	GameSnapshot& operator=(GameSnapshot&&) noexcept = default;
	GameSnapshot(const GameSnapshot&) = delete;
	GameSnapshot& operator=(const GameSnapshot&) = delete;
};



class Game {
private:
	RepetitionTable2 table;

	void checkState() {
		this->gameState = GameState::ONGOING;
		
		// no possible moves.
		if (!MoveGen::hasLegalMoves(*this, this->whiteToMove)) {
			if (this->state.inCheck()) {
				this->gameState = GameState::CHECKMATE;
			}
			else {
				this->gameState = GameState::STALEMATE;
			}
		}
		// check draw.
		else if (this->halfmoves == 50) {
			this->gameState = GameState::DRAW_BY_FIFTY_MOVE_RULE;
		}
		// check insufficient material.
		else if (this->board.hasInsufficientMaterial()) {
			this->gameState = GameState::DRAW_BY_INSUFFICIENT_MATERIAL;
		}
		else if (this->table.isThreefoldRepetition(this->boardHash)) {
			this->gameState = GameState::DRAW_BY_THREEFOLD_REPETITION;
		}
	}
public:
	bool whiteToMove;
	Board board;
	BoardState state;
	uint8_t halfmoves;
	uint8_t fullmoves;
	uint64_t boardHash;
	GameState gameState;
	std::shared_ptr<Player> whitePlayer;
	std::shared_ptr<Player> blackPlayer;

	Game() : whiteToMove(true), gameState(GameState::ONGOING), halfmoves(0), fullmoves(1), boardHash(0) {
		this->whitePlayer = std::make_shared<Human>(Human(true));
		this->blackPlayer = std::make_shared<Human>(Human(false));
	}
	Game(bool whiteBot, bool blackBot) : whiteToMove(true), gameState(GameState::ONGOING), halfmoves(0), fullmoves(1), boardHash(0) {
		if (whiteBot) this->whitePlayer = std::make_shared<Bot>(Bot(true));
		else this->whitePlayer = std::make_shared<Human>(Human(true));
		if (blackBot) this->blackPlayer = std::make_shared<Bot>(Bot(false));
		else this->blackPlayer = std::make_shared<Human>(Human(false));
	}

	void makeBotMove(Move* moves, uint8_t moveCount) {
		if (this->isBotTurn()) {
			this->makeMove(this->whiteToMove ?
				this->whitePlayer->getBestMove(*this, moves, moveCount) :
				this->blackPlayer->getBestMove(*this, moves, moveCount));
		}
	}
	template <bool perft = false, bool search = false>
	void makeMove(Move move) {
		if (move.isCapture || move.piece == Chess::PAWN) {
			this->halfmoves = 0;
			if constexpr (!perft && !search) this->table.eraseTable();
		}
		else this->halfmoves++;

		if (!this->whiteToMove) this->fullmoves++;

		this->board = this->board.branch(move, this->whiteToMove);
		this->whiteToMove = !this->whiteToMove;
		this->init<perft>();
	}

	void undoMove(const GameSnapshot& snapshot) {
		this->whiteToMove = !this->whiteToMove;

		table.remove(this->boardHash);

		this->board = *snapshot.boardPtr;
		this->fullmoves = snapshot.fullmoves;
		this->halfmoves = snapshot.halfmoves;
		this->boardHash = snapshot.boardHash;
		this->gameState = snapshot.gameState;

		this->state.init(this->board, this->whiteToMove);
	}

	template <bool perft = false>
	void init() {
		this->state.init(this->board, this->whiteToMove);

		if constexpr (!perft) {
			this->boardHash = Zobrist::hash(this->board, this->whiteToMove);
			table.store(this->boardHash);
			this->checkState();
		}
	}

	GameSnapshot createSnapshot() const {
		return GameSnapshot(this->halfmoves, this->fullmoves, this->board, this->boardHash, this->gameState);
	}

	friend std::ostream& operator<<(std::ostream& os, const Game& game) {
		os << "Game State: " << (int)game.gameState << '\n';
		os << "Halfmoves: " << (int)game.halfmoves << '\n';
		os << "Fullmoves: " << (int)game.fullmoves << '\n';
		os << "Board Hash: " << std::hex << game.boardHash << std::dec << '\n';
		os << "Board:\n" << game.board;
		os << "Board State:\n" << game.state;
		return os;
	}

	inline bool isBotTurn() const {
		if (this->whiteToMove) return this->whitePlayer->getType() == Type::BOT;
		else return this->blackPlayer->getType() == Type::BOT;
	}

};

#endif
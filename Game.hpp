#ifndef GAME_HPP
#define GAME_HPP


#include <cstdint>
#include "Table.hpp"
#include "Board.hpp"
#include "BoardState.hpp"
#include "Zobrist.hpp"
#include "MoveGen.hpp"

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


public:
	Board board;
	BoardState state;
	uint8_t halfmoves;
	uint8_t fullmoves;
	uint64_t boardHash;
	GameState gameState;

	template <bool whiteToMove>
	void makeMove(const Move& move) {
		if (move.isCapture || move.piece == Chess::PAWN) {
			this->halfmoves = 0;
			this->table.eraseTable();
		}
		else this->halfmoves++;

		if constexpr (!whiteToMove) this->fullmoves++;

		this->board = this->board.branch<whiteToMove>(move);
		this->init<!whiteToMove>();
	}

	template <bool whiteToMove>
	void undoMove(const GameSnapshot& snapshot) {
		table.remove(this->boardHash);

		this->board = *snapshot.boardPtr;
		this->fullmoves = snapshot.fullmoves;
		this->halfmoves = snapshot.halfmoves;
		this->boardHash = snapshot.boardHash;
		this->gameState = snapshot.gameState;

		this->state.init<whiteToMove>(this->board);
	}

	template <bool whiteToMove>
	void init() {
		this->boardHash = Zobrist::hash<whiteToMove>(this->board);
		table.store(this->boardHash);
		this->state.init<whiteToMove>(this->board);
		this->checkState<whiteToMove>();
	}

	GameSnapshot createSnapshot() const {
		return GameSnapshot(this->halfmoves, this->fullmoves, this->board, this->boardHash, this->gameState);
	}

	template <bool whiteToMove>
	void checkState() {
		this->gameState = GameState::ONGOING;

		// no possible moves.
		if (!MoveGen::hasLegalMoves<whiteToMove>(*this)) {
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
		else if (this->board.hasInsufficientMaterial<whiteToMove>() && this->board.hasInsufficientMaterial<!whiteToMove>()) {
			this->gameState = GameState::DRAW_BY_INSUFFICIENT_MATERIAL;
		}
		else if (this->table.isThreefoldRepetition(this->boardHash)) {
			this->gameState = GameState::DRAW_BY_THREEFOLD_REPETITION;
		}
	}

};

#endif
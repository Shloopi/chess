#ifndef INTERMEDIATE_HPP
#define INTERMEDIATE_HPP

#include "const.hpp"
#include "sdl2.hpp"
#include "../Core/Chess.hpp"
#include "../Core/Board.hpp"
#include "../Core/Game.hpp"
#include <array>

using GUIBoard = std::array<std::array<char, Chess::RANK_SIZE>, Chess::FILE_SIZE>;

struct GUIMoves {
	std::array<Move, 218> moveArray;
	uint8_t moveCount;
};
struct GUIPieceMoves {
	Index pieceSquare;
	std::array<Move, 28> moveArray;
	uint8_t moveCount;

	GUIPieceMoves() : pieceSquare(-1), moveCount(0) {}
};


class GUIConverter {
private:
	GUIMoves moves;
	GUIPieceMoves pieceMoves;
	Game game;

	void init();
public:

	GUIConverter() = default;
	GUIConverter(Game& game) : game(game) {
		this->init();
	}

	bool gameOver() {
		return this->game.gameState != GameState::ONGOING;
	}
	bool handleBot();
	void handlePress(GUI::Coord press);
	const char getPiece(Index square);
	Index getPieceMove(uint8_t iteration);

	inline Index getPressedSquare() {
		return this->convertGUI(this->pieceMoves.pieceSquare);
	}
	inline uint8_t getPieceMoveCount() {
		return this->pieceMoves.moveCount;
	}
	inline GUI::Coord getCoord(Index index) {
		return GUI::Coord(Chess::fileOf(index) * GUI::TILE_SIZE, Chess::rankOf(index) * GUI::TILE_SIZE);
	}
	inline Index convertGUI(Index index) {
		if (index == -1) return -1;
		return (Chess::RANK_SIZE - 1 - Chess::rankOf(index)) * Chess::RANK_SIZE + Chess::fileOf(index);
	}
};

#endif
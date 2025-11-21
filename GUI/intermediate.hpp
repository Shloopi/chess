#ifndef INTERMEDIATE_HPP
#define INTERMEDIATE_HPP

#include "const.hpp"
#include "sdl2.hpp"
#include "../Core/Chess.hpp"
#include "../Core/Board.hpp"
#include "../Core/Game.hpp"
#include <array>

using GUIBoard = std::array<std::array<char, Chess::RANK_SIZE>, Chess::FILE_SIZE>;

class GUIConverter {
private:
	Moves<> moves;
	Moves<Move, 28> pieceMoves;
	Index pieceSquare;
	Game game;

	void init();
public:

	GUIConverter() = default;
	GUIConverter(Game& game) : game(game) {
		this->init();
	}

	bool gameOver() const {
		return this->game.gameState != GameState::ONGOING;
	}
	bool handleBot();
	void handlePress(GUI::Coord press);
	const char getPiece(Index square) const;
	Index getPieceMove(uint8_t iteration) const;

	inline Index getPressedSquare() const {
		return this->convertGUI(pieceSquare);
	}
	inline uint8_t getPieceMoveCount() const {
		return this->pieceMoves.count;
	}
	inline GUI::Coord getCoord(Index index) const {
		return GUI::Coord(Chess::fileOf(index) * GUI::TILE_SIZE, Chess::rankOf(index) * GUI::TILE_SIZE);
	}
	inline Index convertGUI(Index index) const {
		if (index == -1) return -1;
		return (Chess::RANK_SIZE - 1 - Chess::rankOf(index)) * Chess::RANK_SIZE + Chess::fileOf(index);
	}
};

#endif
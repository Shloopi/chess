#include "intermediate.hpp"

void GUIConverter::init() {
	this->moves.init();
	this->pieceMoves.init();
	this->pieceSquare = -1;
	MoveGen::genAllLegalMoves(this->game, this->moves);
}

bool GUIConverter::handleBot() {
	if (this->game.isBotTurn()) {
		this->game.makeBotMove(this->moves);
		this->init();
		return true;
	}

	return false;
}

void GUIConverter::handlePress(GUI::Coord press) {
	Index file = press.x / GUI::TILE_SIZE;
	Index rank = press.y / GUI::TILE_SIZE;
	Index index = Chess::toIndex(rank, file);
	index = convertGUI(index);
	
	if (this->game.board.isPieceAt(index, this->game.whiteToMove)) {
		this->pieceSquare = index;
		this->pieceMoves.init();

		for (auto& move : this->moves) {
			if (move.from == index) {
				this->pieceMoves.addMove(move);
			}
		}
	}
	else {
		if (this->pieceSquare != -1) {
			for (auto& move : this->pieceMoves) {
				if (move.to == index) {
					if (!move.isPromotion() || move.flag == Chess::QUEEN_PROMOTION) {
						this->game.makeMove(move);
						this->init();
					}
				}
			}
		}

		this->pieceMoves.init();
		this->pieceSquare = -1;
	}
}

const char GUIConverter::getPiece(Index square) const {
	square = this->convertGUI(square);
	bool isWhite;
	Piece piece = this->game.board.getPieceAt(square, isWhite);
	if (piece != Chess::NO_PIECE) {
		if (isWhite) return Chess::getPiece<true>(piece);
		else return Chess::getPiece<false>(piece);
	}
	else return '.';
}

Index GUIConverter::getPieceMove(uint8_t iteration) const
{
	return this->convertGUI(this->pieceMoves.moves[iteration].to);
}

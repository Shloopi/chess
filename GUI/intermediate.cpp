#include "intermediate.hpp"

void GUIConverter::init() {
	this->pieceMoves.moveCount = 0;
	this->pieceMoves.pieceSquare = -1;
	this->moves.moveCount = MoveGen::genAllLegalMoves(this->game, this->moves.moveArray.data());
}

bool GUIConverter::handleBot() {
	if (this->game.isBotTurn()) {
		this->game.makeBotMove(this->moves.moveArray.data(), this->moves.moveCount);
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
		this->pieceMoves.pieceSquare = index;
		pieceMoves.moveCount = 0;

		for (uint8_t i = 0; i < moves.moveCount; i++) {
			if (this->moves.moveArray[i].from == index) {
				this->pieceMoves.moveArray[pieceMoves.moveCount++] = moves.moveArray[i];
			}
		}
	}
	else {
		if (this->pieceMoves.pieceSquare != -1) {
			for (uint8_t i = 0; i < pieceMoves.moveCount; i++) {
				if (pieceMoves.moveArray[i].to == index) {
					if (!pieceMoves.moveArray[i].isPromotion() || pieceMoves.moveArray[i].flag == Chess::QUEEN_PROMOTION) {
						this->game.makeMove(pieceMoves.moveArray[i]);
						this->init();
					}
				}
			}
		}

		this->pieceMoves.moveCount = 0;
		this->pieceMoves.pieceSquare = -1;
	}
}

const char GUIConverter::getPiece(Index square) {
	square = this->convertGUI(square);
	bool isWhite;
	Piece piece = this->game.board.getPieceAt(square, isWhite);
	if (piece != Chess::NO_PIECE) {
		if (isWhite) return Chess::getPiece<true>(piece);
		else return Chess::getPiece<false>(piece);
	}
	else return '.';
}

Index GUIConverter::getPieceMove(uint8_t iteration)
{
	return this->convertGUI(this->pieceMoves.moveArray[iteration].to);
}

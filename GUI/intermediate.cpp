#include "intermediate.hpp"


namespace GUI {
	void convertBoard(const Board& board, GUI::GUIBoard& guiBoard) {
		Index index;
		Piece piece;
		char pieceChar;
		bool isWhite;

		for (int rank = Chess::RANK_SIZE - 1; rank >= 0; rank--) {
			for (int file = 0; file < Chess::FILE_SIZE; file++) {
				index = Chess::toIndex(Chess::RANK_SIZE - 1 - rank, file);

				
				piece = board.getPieceAt(index, isWhite);

				if (piece != Chess::NO_PIECE) {
					if (isWhite) pieceChar = Chess::getPiece<true>(piece);
					else pieceChar = Chess::getPiece<false>(piece);
				}
				else pieceChar = '.';

				guiBoard[rank][file] = pieceChar;
			}
		}
	}
};
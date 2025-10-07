#include "Board.hpp"
#include "MoveGen.hpp"

const std::string Board::defaultFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

std::ostream& operator<<(std::ostream& os, const Board& board) {
    std::string toPrint = "";

    for (short rank = Chess::RANK_SIZE - 1; rank >= 0; rank--) {
        for (short file = 0; file < Chess::RANK_SIZE; file++) {
            Index square = rank * Chess::RANK_SIZE + file;
            char c;

            if (board.pieceOnSquare(square)) {
                c = PieceHelper::getChar(board.getColoredPieceOnSquare(square));
            }
            else c = '_';

            os << std::string(1, c);
            os << ' ';
        }
        os << '\n';
    }
    os << '\n';

    os << (board.whiteToMove() ? "White" : "Black");
    os << " to move\n";

	os << "Castling rights: ";
    os << (board.info.whiteLeftCastle ? 'K' : '-');
    os << (board.info.whiteRightCastle ? 'Q' : '-');
    os << (board.info.blackLeftCastle ? 'k' : '-');
    os << (board.info.blackRightCastle ? 'q' : '-');
    os << '\n';

    os << "enPassantTarget: ";
    os << Square::getNotation(board.info.enPassantTarget);
    os << '\n';
    os << "halfmoves: ";
    os << board.info.halfmoves;
    os << '\n';
    os << "fullmoves: ";
    os << board.info.fullmoves;
    os << '\n';

    //toPrint += "In Check: " + std::to_string(board.inCheck()) + '\n';

    os << "zobrist: ";
    os << board.boardKey;
    os << '\n';

    return os;
}

void Board::calcPins() {
    Index kingSquare = this->getKingPos();

    // get pseudo moves from the king's square.
    bitboard bishopMoves = MoveGen::getPseudoBishopMoves(*this, kingSquare, this->allPieces);
    bitboard rookMoves = MoveGen::getPseudoRookMoves(*this, kingSquare, this->allPieces);
    bitboard queenMoves = MoveGen::getPseudoQueenMoves(*this, kingSquare, this->allPieces);

    bitboard bishops = this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_BISHOP : ColoredPiece::WHITE_BISHOP];
    bitboard rooks = this->pieces[this->info.whiteMove ? ColoredPiece::WHITE_ROOK : ColoredPiece::BLACK_ROOK];
    bitboard queens = this->pieces[this->info.whiteMove ? ColoredPiece::WHITE_QUEEN : ColoredPiece::BLACK_QUEEN];

    // calculate the potential pieces that can pin pieces (treating queens as being both rooks and bishops).
    bitboard possiblePinningBishops = Constants::BISHOP_ATTACKS[kingSquare] & (bishops | queens) & ~this->checkingPieces;
    bitboard possiblePinningRooks = Constants::ROOK_ATTACKS[kingSquare] & (rooks | queens) & ~this->checkingPieces;

    // check pins for each type (bishops, rooks, queens).
    this->bishopPins = processPins<true>(possiblePinningBishops, bishopMoves);
    this->rookPins = processPins<false>(possiblePinningRooks, rookMoves);
}
template<bool forBishopPins>
bitboard Board::processPins(bitboard possiblePinningPieces, bitboard kingRayMoves) const {
    Index square;
    bitboard pinningPieceMoves, pins = 0ULL;

    // check for each potential pinning piece.
    while (possiblePinningPieces != 0) {
        square = Bitboard::lsb(possiblePinningPieces);
        possiblePinningPieces &= (possiblePinningPieces - 1);

        // get the pseudo moves of that piece.
        if constexpr (forBishopPins)
            pinningPieceMoves = MoveGen::getPseudoBishopMoves(*this, square, this->allPieces);
        else
			pinningPieceMoves = MoveGen::getPseudoRookMoves(*this, square, this->allPieces);

        // if the moves of the piece and the moves from the king overlapps, there is a pinned piece in between.
        pins |= (kingRayMoves & pinningPieceMoves);
    }

    return pins;
}
void Board::calcChecks() {
	Index kingSquare = this->getKingPos();

	this->checkingPieces = 0ULL;

    // knight checks.
	this->checkingPieces |= (Constants::KNIGHT_MOVES[kingSquare] & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_KNIGHT : ColoredPiece::WHITE_KNIGHT]);

    // bishop checks.
    this->checkingPieces |= (MoveGen::getPseudoBishopMoves(*this, kingSquare, this->allPieces) & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_BISHOP : ColoredPiece::WHITE_BISHOP]);
    
    // rook checks.
    this->checkingPieces |= (MoveGen::getPseudoRookMoves(*this, kingSquare, this->allPieces) & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_ROOK : ColoredPiece::WHITE_ROOK]);

    // queen checks.
    this->checkingPieces |= (MoveGen::getPseudoQueenMoves(*this, kingSquare, this->allPieces) & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_QUEEN : ColoredPiece::WHITE_QUEEN]);
    
	// pawn checks.
	this->checkingPieces |= MoveGen::getPawnCaptures(this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_PAWN : ColoredPiece::WHITE_PAWN], !this->info.whiteMove) & Constants::SQUARE_BBS[kingSquare];
}
bool Board::isSquareAttacked(Index square, bitboard pieces) const {
    // knight attacks.
    if ((Constants::KNIGHT_MOVES[square] & this->pieces[this->info.whiteMove ? ColoredPiece::BLACK_KNIGHT : ColoredPiece::WHITE_KNIGHT]) != 0) return true;

    // bishop attacks.
    if ((MoveGen::getPseudoBishopMoves(*this, square, pieces) & this->getPieces(Piece::BISHOP, !this->whiteToMove())) != 0) return true;

    // rook attacks.
    if ((MoveGen::getPseudoRookMoves(*this, square, pieces) & this->getPieces(Piece::ROOK, !this->whiteToMove())) != 0) return true;

    // queen attacks.
    if ((MoveGen::getPseudoQueenMoves(*this, square, pieces) & this->getPieces(Piece::QUEEN, !this->whiteToMove())) != 0) return true;

    // pawn attacks.
    if ((MoveGen::getPawnCaptures(this->getPieces(Piece::PAWN, !this->whiteToMove()), !this->info.whiteMove) & Constants::SQUARE_BBS[square]) != 0) return true;

	return false;
}
void Board::buildPiecesArr() {
    // initiate.
    for (int i = 0; i < this->piecesArr.size(); i++) {
        this->piecesArr[i] = ColoredPiece::COLORED_NONE;
    }

    // for every piece in piece lists, set the piece type at the square index in the array.
    for (unsigned char i = 0; i < this->pieces.size(); i++) {
        bitboard piecesBB = this->pieces[i];

        while (piecesBB != 0) {
			Index square = Bitboard::popLSB(piecesBB);
            this->piecesArr[square] = static_cast<ColoredPiece>(i);
        }
    }
}
void Board::removePiece(ColoredPiece piece, Index sourceSquare, bitboard& coloredPieces) {
	this->pieces[piece] &= ~Constants::SQUARE_BBS[sourceSquare];
    this->allPieces &= ~Constants::SQUARE_BBS[sourceSquare];
	coloredPieces &= ~Constants::SQUARE_BBS[sourceSquare];

    this->piecesArr[sourceSquare] = ColoredPiece::COLORED_NONE;
    this->boardKey = Zobrist::applyPiece(this->boardKey, piece, sourceSquare);
}
void Board::addPiece(ColoredPiece piece, Index targetSquare, bitboard& coloredPieces) {
    this->pieces[piece] |= Constants::SQUARE_BBS[targetSquare];
    this->allPieces |= Constants::SQUARE_BBS[targetSquare];
    coloredPieces |= Constants::SQUARE_BBS[targetSquare];

    this->piecesArr[targetSquare] = piece;
    this->boardKey = Zobrist::applyPiece(this->boardKey, piece, targetSquare);
}
void Board::movePiece(ColoredPiece piece, Index sourceSquare, Index targetSquare, bitboard& coloredPieces) {
    this->removePiece(piece, sourceSquare, coloredPieces);
    this->addPiece(piece, targetSquare, coloredPieces);
}
void Board::makeMove(Move move) {
    // get this friendly and enemy piece bitboards.
    bitboard& turnPieces = this->whiteToMove() ? this->whitePieces : this->blackPieces;
    bitboard& nonTurnPieces = this->whiteToMove() ? this->blackPieces : this->whitePieces;

    bitboard sourceSquareBB = Constants::SQUARE_BBS[move.from];
    bitboard targetSquareBB = Constants::SQUARE_BBS[move.to];

    // store relevent data of current position.
    this->lastBoardInfos.push(this->info);
    this->lastPositions.push(this->boardKey);

    // remove the current board info from the position key.
    this->boardKey = Zobrist::applyBoardInfo(this->boardKey, this->info);

    // handle capture.
    if (move.isCapture()) {
        if (move.type == MoveType::EnPassant) {
            Index capturedPawn = this->whiteToMove() ? move.to - 8 : move.to + 8;
            this->removePiece(PieceHelper::getColoredPieceType(move.capture, !this->whiteToMove()), capturedPawn, nonTurnPieces);
        }
        else {
            // if capturing rook, update castling rights accordingly.
            if (move.capture == Piece::ROOK) {
                if (move.to == Board::whiteLeftRook || move.to == Board::blackLeftRook) {
                    this->info.leftRookMoved();
                }
                else if (move.to == Board::whiteRightRook || move.to == Board::blackRightRook) {
                    this->info.rightRookMoved();
				}
            }
            this->removePiece(PieceHelper::getColoredPieceType(move.capture, !this->whiteToMove()), move.to, nonTurnPieces);
        }
        
    }

    // handle promotion.
    if (move.isPromotion()) {
		this->removePiece(PieceHelper::getColoredPieceType(move.piece, this->whiteToMove()), move.from, turnPieces);
        this->removePiece(PieceHelper::getColoredPieceType(move.promote, this->whiteToMove()), move.to, turnPieces);
    }
    else {
		this->movePiece(PieceHelper::getColoredPieceType(move.piece, this->whiteToMove()), move.from, move.to, turnPieces);
    }


    // handle double pawn push.
    if (move.type == MoveType::DoublePawnPush) {
        this->info.pawnMovedTwoSquares(this->whiteToMove() ? move.to - 8 : move.to + 8);
    }
    else {
        this->info.pawnDidntMoveTwoSquares();
    }

    if (move.piece == Piece::KING) {
        this->info.kingMoved();

        // handle castling.
        if (move.isCastling()) {
            Index sourceRookSquare, targetRookSquare;

            if (this->whiteToMove()) {
                if (move.type == MoveType::KingCastle) {
					sourceRookSquare = Board::whiteRightRook;
                    targetRookSquare = Board::whiteRightRookCastle;
                }
                else {
                    sourceRookSquare = Board::whiteLeftRook;
					targetRookSquare = Board::whiteLeftRookCastle;
                }
            }
            else {
                if (move.type == MoveType::KingCastle) {
                    sourceRookSquare = Board::blackRightRook;
					targetRookSquare = Board::blackRightRookCastle;
                }
                else {
                    sourceRookSquare = Board::blackLeftRook;
					targetRookSquare = Board::blackLeftRookCastle;
                }
            }

            this->movePiece(PieceHelper::getColoredPieceType(Piece::ROOK, this->whiteToMove()), sourceRookSquare, targetRookSquare, turnPieces);
        }
    }
    // handle castling rights after rook moves.
    else if (move.piece == Piece::ROOK) {
        if (move.to == Board::whiteLeftRook || move.to == Board::blackLeftRook) {
            this->info.leftRookMoved();
        }
        else if (move.to == Board::whiteRightRook || move.to == Board::blackRightRook) {
            this->info.rightRookMoved();
        }
    }

    // handle halfmove counter.
    if (move.isCapture() || move.piece == Piece::PAWN) this->info.halfmoves = 0;
    else this->info.halfmoves++;

    // push the move.
    this->lastMoves.push(move);

    // add the new board info to the position key.
    this->boardKey = Zobrist::applyBoardInfo(this->boardKey, this->info);

    // reverse the color.
    this->boardKey = Zobrist::applyTurn(this->boardKey, this->info.whiteMove);
	this->info.toggleTurn();
    this->boardKey = Zobrist::applyTurn(this->boardKey, this->info.whiteMove);

    // handle position repetition.
    this->positionsRep[this->boardKey]++;
    this->threefoldRepetition = this->positionsRep[this->boardKey] >= 3;

    this->allPieces = this->whitePieces | this->blackPieces;
    this->calcChecks();
    this->calcPins();

    // check end.
    //this->info.endState = GameResult::checkState(*this);
}

#include "MoveGen.hpp"
#include "BoardState.hpp"

namespace MagicGen {
    void genOccupancies(std::vector<bitboard>& occupancies, bitboard moves) {
		bitboard currOccupancy = moves;

        for (int i = 0; i < occupancies.size(); i++) {
            occupancies[i] = currOccupancy;
            currOccupancy = (currOccupancy - 1) & moves;
        }
    }

    bitboard genSlidingAttack(bool forBishop, Square sourceSquare, bitboard occupancy) {
        auto& directions = forBishop ? Directions::bishopDirections : Directions::rookDirections;

        bitboard currMove, attacks = 0ULL;
        Square targetSquare;
        for (int i = 0; i < directions.size(); i++) {
			targetSquare = Square(sourceSquare.file, sourceSquare.rank);
            targetSquare.addSquares(directions[i]);
            int x = 0;
            while (targetSquare.isValid()) {
                currMove = Constants::SQUARE_BBS[targetSquare.index];
                attacks |= currMove;
                targetSquare.addSquares(directions[i]);

                if ((currMove & occupancy) != 0) {
                    break;
                }


				x++;
            }
        }

        return attacks;
    }

    void genMagicTable(bool forBishop) {
        auto& magicTable = forBishop ? bishopMagicTable : rookMagicTable;
        auto& shifters = forBishop ? Constants::BISHOP_SHIFTERS : Constants::ROOK_SHIFTERS;
        auto& magicNumbers = forBishop ? Constants::BISHOP_MAGIC_NUMBERS : Constants::ROOK_MAGIC_NUMBERS;
        auto& attacks = forBishop ? Constants::BISHOP_ATTACKS_NO_LAST_SQUARE : Constants::ROOK_ATTACKS_NO_LAST_SQUARE;

        bitboard moves, attack;
        std::vector<bitboard> occupancies;
        Square sourceSquare;
        Index lookupTableIndex;

        for (int i = 0; i < attacks.size(); i++) {
            sourceSquare = Square(i);
            moves = attacks[i];
            if (forBishop) moves &= ~Chess::RANK1 & ~Chess::RANK8 & ~Chess::FILE_A & ~Chess::FILE_H;
            

            magicTable[i].resize(Constants::SQUARE_BBS[Chess::BOARD_SIZE - shifters[i]]);

			// Generate all possible occupancies for the given moves.
            std::vector<bitboard> occupancies(Constants::SQUARE_BBS[Chess::numOfBits(moves)]);
			genOccupancies(occupancies, moves);

            for (int j = 0; j < occupancies.size(); j++) {
                lookupTableIndex = genMagicIndex(occupancies[j], magicNumbers[i], shifters[i]);
                attack = genSlidingAttack(forBishop, sourceSquare, occupancies[j]);

                magicTable[i][lookupTableIndex] = attack;
            }
        }
    }
}

namespace MoveGen {

    template <bool whiteToMove>
    bitboard movesLegalityWhileChecked(const BoardState& state, Index sourceSquare, bitboard targetSquares) {
        // if there is no check, return all moves.
        if (!state.inCheck()) return targetSquares;

        // if there are 2 checks, only king moves are legal.
        if (state.numOfChecks() > 1) return 0ULL;

        // calculate if moves are legal when there is one checking piece.
        bitboard legalMoves = 0ULL;
        Index kingSquare = state.board.getKing<whiteToMove>();

        // capturing the enemy checking piece.
        legalMoves |= (targetSquares & checkingPiece);

        // blocking the ray.
        bitboard checkRay = Constants::BETWEEN_PIECES_TABLE[kingSquare][Chess::lsb(checkingPiece)];

        legalMoves |= (checkRay & targetSquares);

        return legalMoves;
    }

	template <bool whiteToMove>
    bitboard reducePinnedPiecesMoves(const BoardState& state, Index sourceSquare, bitboard targetSquares) {
        if (!state.isPiecePinned(sourceSquare)) return targetSquares;

        bitboard moveRay, temp = targetSquares, kingSquare = state.board.getKing<whiteToMove>();
        Index targetSquare;


        while (temp != 0ULL) {
            targetSquare = Chess::popLSB(temp);
            moveRay = Constants::BETWEEN_TABLE[sourceSquare][targetSquare];

            if ((kingSquare & moveRay) == 0ULL) targetSquares &= ~Constants::SQUARE_BBS[targetSquare];
        }

        return targetSquares;
    }

	template <bool whiteToMove>
    bool enPassantExposeKing(const Board& board, bitboard enPassantTarget, bitboard capturingPawn) {
        bitboard allPieces = board.getAllPieces();
        // king pos
        Index kingPos = board.getKing<whiteToMove>();
		bitboard king = Constants::SQUARE_BBS[kingPos];

        bitboard capturedPawn = Chess::pawnBackward<whiteToMove>(enPassantTarget);

        // get the rank of the capturing and captured pawns.
        bitboard rankBB = Chess::enPassantRank<whiteToMove>();
        
        if ((king & rankBB) == 0) return false;

        // calculate the king's ray.
        bitboard kingRay = MoveGen::getPseudoRookMoves(board, kingPos, allPieces) & rankBB;
        if ((kingRay & capturingPawn) == 0 && (kingRay & capturedPawn) == 0) return false;

        bitboard movesBitboard, rookRayPieces = (board.getRooks<!whiteToMove>() | board.getQueens<!whiteToMove>()) & rankBB;
        Index square;

        while (rookRayPieces != 0) {
            square = Chess::popLSB(rookRayPieces);

            movesBitboard = MoveGen::getPseudoRookMoves(board, square, allPieces) & rankBB;

            if ((movesBitboard & capturingPawn) != 0 || (movesBitboard & capturedPawn) != 0) return true;
        }

        return false;
    }

	template <bool whiteToMove>
    bitboard genBitboardLegalPawnMoves(const BoardState& state, bitboard pawns) {
        bitboard singlePushes, doublePushes, leftCaptures, rightCaptures;
        MoveGen::genBitboardsLegalPawnMoves<whiteToMove>(state, pawns, singlePushes, doublePushes, leftCaptures, rightCaptures);

        return singlePushes | doublePushes | leftCaptures | rightCaptures;
    }

	template <bool whiteToMove>
    void genBitboardsLegalPawnMoves(const BoardState& state, bitboard pawns, bitboard& singlePushes, bitboard& doublePushes, bitboard& leftCaptures, bitboard& rightCaptures) {
        bitboard singlePushesTemp, doublePushesTemp, leftCapturesTemp, rightCapturesTemp;
        bitboard enPassantBitboard = state.board.enPassant, square;
        bitboard emptySquares = state.board.getFreeSquares(), enemyPieces = state.board.getEnemyPieces<whiteToMove>();

        // get the checking ray.
        bitboard checkRay = state.inCheck() ? state.checkingPieces | Constants::BETWEEN_PIECES_TABLE[board.getKingPos()][board.getCheckingPiecePos()] : Chess::MAX_BITBOARD;

        // get non pinned pieces.
        bitboard nonPinnedPawns = pawns & ~state.getPinnedPieces();

        // generate single and double pushes, left and right captures for non pinned pawns.
        // if in check, keep moves that block or captures the checking piece.
        singlePushes = Chess::pawnForward<whiteToMove>(nonPinnedPawns) & emptySquares;
        doublePushes = Chess::pawnForward<whiteToMove>(singlePushes) & emptySquares & Chess::doublePushRank<whiteToMove>() & checkRay;
        singlePushes &= checkRay;
        leftCaptures = Chess::pawnAttackLeft<whiteToMove>(nonPinnedPawns) & Chess::pawnLeftMask<whiteToMove>() & (enemyPieces | enPassantBitboard) & checkRay;
        rightCaptures = Chess::pawnAttackRight<whiteToMove>(nonPinnedPawns) & Chess::pawnRightMask<whiteToMove>() & (enemyPieces | enPassantBitboard) & checkRay;

        // generate pinned pawn moves.
        bitboard pinnedPawns = pawns & state.getPinnedPieces();
        bitboard pinnedPawn;

        while (pinnedPawns != 0) {
            square = Chess::popLSB(pinnedPawns);
            pinnedPawn = Constants::SQUARE_BBS[square];

            // get single, double and captures for this pinned pawn.
            singlePushesTemp = Chess::pawnForward<whiteToMove>(pinnedPawn) & emptySquares;
            doublePushesTemp = Chess::pawnForward<whiteToMove>(singlePushesTemp) & emptySquares & Chess::doublePushRank<whiteToMove>() & checkRay;
            singlePushesTemp &= checkRay;
            leftCapturesTemp = Chess::pawnAttackLeft<whiteToMove>(pinnedPawn) & Chess::pawnLeftMask<whiteToMove>() & (enemyPieces | enPassantBitboard) & checkRay;
            rightCapturesTemp = Chess::pawnAttackRight<whiteToMove>(pinnedPawn) & Chess::pawnRightMask<whiteToMove>() & (enemyPieces | enPassantBitboard) & checkRay;

            rightCapturesTemp = MoveGen::calcRightPawnCaptures(pinnedPawn, enemyPieces | enPassantBitboard, board.whiteToMove()) & checkRay;

            // remove moves that does not block check or move away from a pin.
            singlePushesTemp = MoveGen::reducePinnedPiecesMoves(board, square, singlePushesTemp);
            singlePushes |= singlePushesTemp;

            // remove moves that does not block check or move away from a pin.
            doublePushesTemp = MoveGen::reducePinnedPiecesMoves(board, square, doublePushesTemp);
            doublePushes |= doublePushesTemp;

            // remove moves that does not block check or move away from a pin.
            leftCapturesTemp = MoveGen::reducePinnedPiecesMoves(board, square, leftCapturesTemp);
            leftCaptures |= leftCapturesTemp;

            // remove moves that does not block check or move away from a pin.
            rightCapturesTemp = MoveGen::reducePinnedPiecesMoves(board, square, rightCapturesTemp);
            rightCaptures |= rightCapturesTemp;
        }

        // check if en passant move is exposing the king.
        if ((leftCaptures & enPassantBitboard) != 0) {
            bitboard capturingPawn = board.whiteToMove() ? enPassantBitboard >> 7 : enPassantBitboard << 9;
            if (MoveGen::enPassantExposeKing(board, enPassantBitboard, capturingPawn)) leftCaptures &= ~enPassantBitboard;
        }
        else if ((rightCaptures & enPassantBitboard) != 0) {
            bitboard capturingPawn = board.whiteToMove() ? enPassantBitboard >> 9 : enPassantBitboard << 7;
            if (MoveGen::enPassantExposeKing(board, enPassantBitboard, capturingPawn)) rightCaptures &= ~enPassantBitboard;
        }
    }

	template <bool whiteToMove>
    bitboard genLegalKingMoves(const BoardState& state, Index square) {
        bitboard pieces = state.board.getAllPieces();
        bitboard noKingPieces = pieces & ~Constants::SQUARE_BBS[square];
        bitboard moveBitboard = MoveGen::getPseudoKingMoves<whiteToMove>(state.board, square);
        bitboard tempBitboard = moveBitboard;
        Index targetSquare, targetCastlingSquare;

        // run for every move and check if the target square is attacked.
        while (tempBitboard != 0) {
            // get the index of the target square and remove it from the bitboard.
            targetSquare = Chess::popLSB(tempBitboard);

            // if the target square is attacked, remove it from the moves bitboard.
            if (state.isSquareAttacked(targetSquare, noKingPieces)) moveBitboard &= ~Constants::SQUARE_BBS[targetSquare];
        }
        // TODO - i am here
        // check for kingside castling.
        targetCastlingSquare = square + 2;

        if (!board.inCheck() && board.canCastleKingside() && (moveBitboard & (1ULL << (square + 1))) != 0 && (pieces & (1ULL << targetCastlingSquare)) == 0 && (pieces & (1ULL << (square + 1))) == 0 && !board.isSquareAttacked(targetCastlingSquare, board.getAllPieces())) {
            moveBitboard |= (1ULL << targetCastlingSquare);
        }

        // check for queenside castling.
        targetCastlingSquare = square - 2;
        if (!board.inCheck() && board.canCastleQueenside() && (moveBitboard & (1ULL << (square - 1))) != 0 && (pieces & (1ULL << targetCastlingSquare)) == 0 && (pieces & (1ULL << (square - 1))) == 0 && !board.isSquareAttacked(targetCastlingSquare, board.getAllPieces())) {
            moveBitboard |= (1ULL << targetCastlingSquare);
        }
        return moveBitboard;
    }

    template <bool whiteToMove, Piece piece>
    bitboard genLegalMoves(const BoardState& state, Index square) {
        bitboard movesBitboard = 0ULL;

        // if there are 2 checks, only king moves are legal.
        if constexpr (piece != Chess::KING) {
            if (state.numOfChecks() > 1) {
                return 0ULL;
            }
        }

        if constexpr (piece == Chess::PAWN) return MoveGen::genBitboardLegalPawnMoves<whiteToMove>(state, Constants::SQUARE_BBS[square]);
        if constexpr (piece == Chess::QUEEN) movesBitboard = MoveGen::getPseudoQueenMoves<whiteToMove>(state.board, square, state.board.getAllPieces());
        if constexpr (piece == Chess::KING) return MoveGen::genLegalKingMoves<whiteToMove>(state, square);
        if constexpr (piece == Chess::KNIGHT) {
            // a pinned knight can't move.
            if (state.isPiecePinned(square)) return 0ULL;

            movesBitboard = MoveGen::getPseudoKnightMoves<whiteToMove>(state.board, square);
        }
        if constexpr (T == Chess::BISHOP) {
            // a bishop that is pinned by a rook ray, can't move.
            if (state.isPinnedByRook(square)) return 0ULL;

            movesBitboard = MoveGen::getPseudoBishopMoves<whiteToMove>(state.board, square, state.board.getAllPieces());
        }
        if constexpr (T == Chess::ROOK) {
            // a rook that is pinned by a bishop ray, can't move.
            if (state.isPinnedByBishop(square)) return 0ULL;

            movesBitboard = MoveGen::getPseudoRookMoves<whiteToMove>(state.board, square, state.board.getAllPieces());
        }

        // reduce moves because of check.
        movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(state, square, movesBitboard);

        // check pins and reduce moves.
        movesBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(state, square, movesBitboard);

        return movesBitboard;
    }
    template <bool whiteToMove, Piece piece>
    void insertMoves(const Board& board, Move* moves, unsigned short& moveCount, Index sourceSquare, bitboard moveBitboard) {
        Index square;
        Flag flag;
        const bitboard enemyPieces = board.getEnemyPieces();
        Move* out = moves + moveCount;

        while (moveBitboard != 0ULL) {
            // get index of least-significant bit and remove the bit. 
            square = Chess::popLSB(moveBitboard);

            flag = MoveGen::calcFlag<whiteToMove, piece>(sourceSquare, square, enemyPieces);

            *out++ = Move(sourceSquare, square, piece, flag);
        }

        moveCount = out - moves;
    }
    template<bool whiteToMove, Piece piece>
    Flag calcFlag(Index sourceSquare, Index targetSquare, const bitboard& enemyPieces) {
        bitboard sourceSquareBB = Constants::SQUARE_BBS[sourceSquare];

        if constexpr (piece == Chess::KNIGHT || piece == Chess::BISHOP || piece == Chess::QUEEN) {
            return Chess::QUIET;
        }
        else if constexpr (piece == Chess::ROOK) {
            if ((Board::startingKingsideRook<whiteToMove>() & sourceSquareBB) != 0) {
                return Chess::REMOVE_SHORT_CASTLING;
            }
            else if ((Board::startingQueensideRook<whiteToMove>() & sourceSquareBB) != 0) {
                return Chess::REMOVE_LONG_CASTLING;
            }
            else {
                return Chess::QUIET;
            }
        }
        else if constexpr (piece == Chess::KING) {
            if ((targetSquare - sourceSquare) == 2) return Chess::SHORT_CASTLING;
            else if ((sourceSquare - targetSquare) == 2) return Chess::LONG_CASTLING;
            else return Chess::REMOVE_ALL_CASTLING;
        }
        else {
            return Chess::QUIET;
        }
    }

	template <bool whiteToMove>
    void insertPawnMoves(const Board& board, Move* moves, unsigned short& moveCount, bitboard moveBitboard, short StartSquareDelta, Flag flag) {
        Index square, sourceSquare;

        bitboard promoMoves = moveBitboard & Chess::promotionRank<whiteToMove>();
        bitboard quietMoves = moveBitboard ^ promoMoves;
        Index enPassant = board.getEnPassantSquare();

        Move* out = moves + moveCount;

        while (quietMoves != 0ULL) {
            // get index of least-significant bit and remove the bit. 
            square = Chess::popLSB(quietMoves);
            sourceSquare = square - StartSquareDelta;

            if (square == enPassant) {
                *out++ = Move(sourceSquare, square, Chess::PAWN, Chess::EN_PASSANT);
            }
            else {
                *out++ = Move(sourceSquare, square, Chess::PAWN, flag);
            }
        }

        while (promoMoves != 0ULL) {
            // get index of least-significant bit and remove the bit. 
            square = Chess::popLSB(promoMoves);
            sourceSquare = square - StartSquareDelta;

            *out++ = Move(sourceSquare, square, Chess::PAWN, Chess::KNIGHT_PROMOTION);
            *out++ = Move(sourceSquare, square, Chess::PAWN, Chess::BISHOP_PROMOTION);
            *out++ = Move(sourceSquare, square, Chess::PAWN, Chess::ROOK_PROMOTION);
            *out++ = Move(sourceSquare, square, Chess::PAWN, Chess::QUEEN_PROMOTION);
        }

        moveCount = out - moves;
    }

    template <bool whiteToMove>
    void genLegalPawnMoves(const Board& board, bitboard pawns, Move* moves, unsigned short& moveCount) {
        if (pawns != 0ULL) {
            // get bitboard moves.
            bitboard singlePushes, doublePushes, leftCaptures, rightCaptures;
            MoveGen::genBitboardsLegalPawnMoves<whiteToMove>(board, pawns, singlePushes, doublePushes, leftCaptures, rightCaptures);
            MoveGen::insertPawnMoves<whiteToMove>(board, moves, moveCount, singlePushes, whiteToMove ? 8 : -8, MoveType::Quiet);
            MoveGen::insertPawnMoves<whiteToMove>(board, moves, moveCount, doublePushes, whiteToMove ? 16 : -16, MoveType::DoublePawnPush);
            MoveGen::insertPawnMoves<whiteToMove>(board, moves, moveCount, leftCaptures, whiteToMove ? 7 : -9, MoveType::Capture);
            MoveGen::insertPawnMoves<whiteToMove>(board, moves, moveCount, rightCaptures, whiteToMove ? 9 : -7, MoveType::Capture);
        }
    }

    template <bool whiteToMove>
    void genLegalHumanMoves(const BoardState& state, Index square, Move* moves, unsigned short& moveCount) {
        Piece type = board.getPieceAt(square);
        bitboard movesBitboard;

        if (type == Chess::PAWN) {
            MoveGen::genLegalPawnMoves(state.board, (1ULL << square), moves, moveCount);
        }
        else {
            if (type == Chess::KNIGHT) {
                movesBitboard = MoveGen::genLegalMoves<whiteToMove, Chess::KNIGHT>(state, square);
                MoveGen::insertMoves<whiteToMove, Chess::KNIGHT>(state.board, moves, moveCount, square, movesBitboard);
            }
            if (type == Chess::BISHOP) {
                movesBitboard = MoveGen::genLegalMoves<whiteToMove, Chess::BISHOP>(state, square);
                MoveGen::insertMoves<whiteToMove, Chess::BISHOP>(state.board, moves, moveCount, square, movesBitboard);
            }
            if (type == Chess::ROOK) {
                movesBitboard = MoveGen::genLegalMoves<whiteToMove, Chess::ROOK>(state, square);
                MoveGen::insertMoves<whiteToMove, Chess::ROOK>(state.board, moves, moveCount, square, movesBitboard);
            }
            if (type == Chess::QUEEN) {
                movesBitboard = MoveGen::genLegalMoves<whiteToMove, Chess::QUEEN>(state, square);
                MoveGen::insertMoves<whiteToMove, Chess::QUEEN>(state.board, moves, moveCount, square, movesBitboard);
            }
            if (type == Chess::KING) {
                movesBitboard = MoveGen::genLegalMoves<whiteToMove, Chess::KING>(state, square);
                MoveGen::insertMoves<whiteToMove, Chess::KING>(state.board, moves, moveCount, square, movesBitboard);
            }
        }
    }

	template <bool whiteToMove>
    unsigned short genAllLegalMoves(const BoardState& state, Move* moves) {
        unsigned short moveCount = 0;

        if (state.numOfChecks() > 1) {
            MoveGen::genLegalHumanMoves<whiteToMove>(state, state.board.getKing(), moves, moveCount);
            return moveCount;
        }

        // get all pawns.
        bitboard pawns = state.board.getPawns<whiteToMove>();

        // insert pawn moves.
        MoveGen::genLegalPawnMoves<whiteToMove>(state.board, pawns, moves, moveCount);

        // get all other pieces.
        bitboard nonPawnPieces = state.board.getAllPieces<whiteToMove>() & ~pawns;

        Index square;

        // insert all other piece moves.
        while (nonPawnPieces != 0) {
            square = Chess::popLSB(nonPawnPieces);

            MoveGen::genLegalHumanMoves<whiteToMove>(state, square, moves, moveCount);
        }

        return moveCount;
    }

    template <bool whiteToMove>
    bool hasLegalMoves(const BoardState& state) {
        if (state.numOfChecks() > 1) return MoveGen::genLegalKingMoves<whiteToMove>(state, state.board.getKing()) != 0;

        // get all pawns.
        bitboard pawns = state.board.getPawns<whiteToMove>();

        // insert pawn moves.
        bitboard moves = MoveGen::genBitboardLegalPawnMoves<whiteToMove>(state, pawns);
        if (moves != 0) return true;

        // get all other pieces.
        bitboard nonPawnPieces = state.board.getAllPieces<whiteToMove>() & ~pawns;

        Index square;
        Piece piece;

        // insert all other piece moves.
        while (nonPawnPieces != 0) {
            square = Chess::popLSB(nonPawnPieces);
            
            piece = state.board.getPieceAt(square);

            if (piece == Chess::KNIGHT) moves = MoveGen::genLegalMoves<whiteToMove, Chess::KNIGHT>(state, square);
            else if (piece == Chess::BISHOP) moves = MoveGen::genLegalMoves<whiteToMove, Chess::BISHOP>(state, square);
            else if (piece == Chess::ROOK) moves = MoveGen::genLegalMoves<whiteToMove, Chess::ROOK>(state, square);
            else if (piece == Chess::QUEEN) moves = MoveGen::genLegalMoves<whiteToMove, Chess::QUEEN>(state, square);
            else if (piece == Chess::KING) moves = MoveGen::genLegalMoves<whiteToMove, Chess::KING>(state, square);
            if (moves != 0) return true;

        }

        return false;
    }
}
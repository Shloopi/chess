#include "MoveGen.hpp"

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
            if (forBishop) moves &= ~Bitboard::RANK0 & ~Bitboard::RANK7 & ~Bitboard::FILE_A & ~Bitboard::FILE_H;
            

            magicTable[i].resize(Constants::SQUARE_BBS[Chess::BOARD_SIZE - shifters[i]]);

			// Generate all possible occupancies for the given moves.
            std::vector<bitboard> occupancies(Constants::SQUARE_BBS[Bitboard::numOfBits(moves)]);
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
    bitboard MoveGen::movesLegalityWhileChecked(const Board& board, Index sourceSquare, bitboard targetSquares) {
        // if there is no check, return all moves.
        if (!board.inCheck()) return targetSquares;

        // if there are 2 checks, only king moves are legal.
        if (board.numOfChecks() > 1) return 0ULL;

        // calculate if moves are legal when there is one checking piece.
        bitboard legalMoves = 0ULL, checkingPiece = board.getCheckingPieces();
        Index kingSquare = board.getKingPos();

        // capturing the enemy checking piece.
        legalMoves |= (targetSquares & checkingPiece);

        // blocking the ray.
        bitboard checkRay = Constants::BETWEEN_PIECES_TABLE[kingSquare][Bitboard::lsb(checkingPiece)];

        legalMoves |= (checkRay & targetSquares);

        return legalMoves;
    }
    bitboard MoveGen::reducePinnedPiecesMoves(const Board& board, Index sourceSquare, bitboard targetSquares) {
        if (!board.isPiecePinned(sourceSquare)) return targetSquares;

        bitboard moveRay, temp = targetSquares, kingSquare = board.getKingPos();
        Index targetSquare;


        while (temp != 0ULL) {
            targetSquare = Bitboard::popLSB(temp);
            moveRay = Constants::BETWEEN_TABLE[sourceSquare][targetSquare];

            if ((kingSquare & moveRay) == 0ULL) targetSquares &= ~(1ULL << targetSquare);
        }

        return targetSquares;
    }
    bool MoveGen::enPassantExposeKing(const Board& board, bitboard enPassantTarget, bitboard capturingPawn) {
        // king pos
        Index kingPos = board.getKingPos();
		bitboard king = Constants::SQUARE_BBS[kingPos];

        bitboard capturedPawn = board.whiteToMove() ? enPassantTarget >> 8 : enPassantTarget << 8;

        // get the rank of the capturing and captured pawns.
        bitboard rankBB = board.whiteToMove() ? Bitboard::RANK4 : Bitboard::RANK3;

        if ((king & rankBB) == 0) return false;

        // calculate the king's ray.
        bitboard kingRay = MoveGen::getPseudoRookMoves(board, kingPos, board.getAllPieces()) & rankBB;
        if ((kingRay & capturingPawn) == 0 && (kingRay & capturedPawn) == 0) return false;

        bitboard movesBitboard, rookRayPieces = (board.getPieces(Piece::ROOK, !board.whiteToMove()) | board.getPieces(Piece::QUEEN, board.whiteToMove())) & rankBB;
        Index square;

        while (rookRayPieces != 0) {
            square = Bitboard::popLSB(rookRayPieces);

            movesBitboard = MoveGen::getPseudoRookMoves(board, square, board.getAllPieces()) & rankBB;

            if ((movesBitboard & capturingPawn) != 0 || (movesBitboard & capturedPawn) != 0) return true;
        }

        return false;
    }
    bitboard MoveGen::genBitboardLegalPawnMoves(const Board& board, bitboard pawns) {
        bitboard singlePushes, doublePushes, leftCaptures, rightCaptures;
        MoveGen::genBitboardsLegalPawnMoves(board, pawns, singlePushes, doublePushes, leftCaptures, rightCaptures);

        return singlePushes | doublePushes | leftCaptures | rightCaptures;
    }
    void MoveGen::genBitboardsLegalPawnMoves(const Board& board, bitboard pawns, bitboard& singlePushes, bitboard& doublePushes, bitboard& leftCaptures, bitboard& rightCaptures) {
        bitboard singlePushesTemp, doublePushesTemp, leftCapturesTemp, rightCapturesTemp;
        Index enPassantTarget = board.getEnPassantTarget(), square;
        bitboard emptySquares = ~board.getAllPieces(), enemyPieces = board.getEnemyPieces(), enPassantBitboard = enPassantTarget == 64 ? 0ULL : (1ULL << enPassantTarget);

        // get the checking ray.
        bitboard checkRay = board.inCheck() ? board.getCheckingPieces() | Constants::BETWEEN_PIECES_TABLE[board.getKingPos()][board.getCheckingPiecePos()] : Bitboard::MAX_BITBOARD;

        // get non pinned pieces.
        bitboard nonPinnedPawns = pawns & ~board.getPinnedPieces();

        // generate single and double pushes, left and right captures for non pinned pawns.
        // if in check, keep moves that block or captures the checking piece.
        singlePushes = MoveGen::calcSinglePawnPushes(nonPinnedPawns, emptySquares, board.whiteToMove());
        doublePushes = MoveGen::calcDoublePawnPushes(singlePushes, emptySquares, board.whiteToMove()) & checkRay;
        singlePushes &= checkRay;
        leftCaptures = MoveGen::calcLeftPawnCaptures(nonPinnedPawns, enemyPieces | enPassantBitboard, board.whiteToMove()) & checkRay;
        rightCaptures = MoveGen::calcRightPawnCaptures(nonPinnedPawns, enemyPieces | enPassantBitboard, board.whiteToMove()) & checkRay;

        // generate pinned pawn moves.
        bitboard pinnedPawns = pawns & board.getPinnedPieces();
        bitboard pinnedPawn;

        while (pinnedPawns != 0) {
            square = Bitboard::popLSB(pinnedPawns);
            pinnedPawn = Constants::SQUARE_BBS[square];

            // get single, double and captures for this pinned pawn.
            singlePushesTemp = MoveGen::calcSinglePawnPushes(pinnedPawn, emptySquares, board.whiteToMove());
            doublePushesTemp = MoveGen::calcDoublePawnPushes(singlePushesTemp, emptySquares, board.whiteToMove()) & checkRay;
            singlePushesTemp &= checkRay;
            leftCapturesTemp = MoveGen::calcLeftPawnCaptures(pinnedPawn, enemyPieces | enPassantBitboard, board.whiteToMove()) & checkRay;
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
    bitboard MoveGen::genLegalKingMoves(const Board& board, Index square) {
        bitboard pieces = board.getAllPieces();
        bitboard noKingPieces = pieces & ~(1ULL << square);
        bitboard moveBitboard = MoveGen::getPseudoKingMoves(board, square);
        bitboard tempBitboard = moveBitboard;
        Index targetSquare, targetCastlingSquare;

        // run for every move and check if the target square is attacked.
        while (tempBitboard != 0) {
            // get the index of the target square and remove it from the bitboard.
            targetSquare = Bitboard::popLSB(tempBitboard);

            // if the target square is attacked, remove it from the moves bitboard.
            if (board.isSquareAttacked(targetSquare, noKingPieces)) moveBitboard &= ~(1ULL << targetSquare);
        }

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
    template <Piece T>
    bitboard genLegalMoves(const Board& board, Index square) {
        bitboard movesBitboard = 0ULL;

        // if there are 2 checks, only king moves are legal.
        if constexpr (T != Piece::KING) {
            if (board.numOfChecks() > 1) {
                return 0ULL;
            }
        }

        if constexpr (T == Piece::NONE) return 0ULL;
        if constexpr (T == Piece::PAWN) return genBitboardLegalPawnMoves(board, Constants::SQUARE_BBS[square]);
        if constexpr (T == Piece::QUEEN) movesBitboard = MoveGen::getPseudoQueenMoves(board, square, board.getAllPieces());
        if constexpr (T == Piece::KING) return MoveGen::genLegalKingMoves(board, square);
        if constexpr (T == Piece::KNIGHT) {
            // a pinned knight can't move.
            if (board.isPiecePinned(square)) return 0ULL;

            movesBitboard = MoveGen::getPseudoKnightMoves(board, square);
        }
        if constexpr (T == Piece::BISHOP) {
            // a bishop that is pinned by a rook ray, can't move.
            if (board.isPinnedByRook(square)) return 0ULL;

            movesBitboard = MoveGen::getPseudoBishopMoves(board, square, board.getAllPieces());
        }
        if constexpr (T == Piece::ROOK) {
            // a rook that is pinned by a bishop ray, can't move.
            if (board.isPinnedByBishop(square)) return 0ULL;

            movesBitboard = MoveGen::getPseudoRookMoves(board, square, board.getAllPieces());
        }

        // reduce moves because of check.
        movesBitboard = MoveGen::movesLegalityWhileChecked(board, square, movesBitboard);

        // check pins and reduce moves.
        movesBitboard = MoveGen::reducePinnedPiecesMoves(board, square, movesBitboard);

        return movesBitboard;
    }
    template <Piece T>
    void insertMoves(const Board& board, Move* moves, unsigned short& moveCount, Index sourceSquare, bitboard moveBitboard) {
        Index square;
        MoveType moveType;
        const bitboard enemyPieces = board.getEnemyPieces();
        Move* out = moves + moveCount;

        while (moveBitboard != 0ULL) {
            // get index of least-significant bit and remove the bit. 
            square = Bitboard::popLSB(moveBitboard);

            moveType = MoveGen::calcMoveType<T>(sourceSquare, square, enemyPieces);

            if (moveType == MoveType::Capture) {
                *out++ = Move(sourceSquare, square, T, moveType, board.getPieceOnSquare(square));
            }
            else {
                *out++ = Move(sourceSquare, square, T, moveType);
            }
        }

        moveCount = out - moves;
    }
    template<Piece T>
    MoveType calcMoveType(Index sourceSquare, Index targetSquare, const bitboard& enemyPieces) {
        bitboard targetSquareBB = 1ULL << targetSquare;

        if constexpr (T == Piece::KNIGHT || T == Piece::BISHOP ||
            T == Piece::ROOK || T == Piece::QUEEN) {
            return (enemyPieces & targetSquareBB) ? MoveType::Capture : MoveType::Quiet;
        }
        else if constexpr (T == Piece::KING) {
            if (enemyPieces & targetSquareBB) return MoveType::Capture;
            else if ((targetSquare - sourceSquare) == 2) return MoveType::KingCastle;
            else if ((sourceSquare - targetSquare) == 2) return MoveType::QueenCastle;
            else return MoveType::Quiet;
        }
        else {
            return MoveType::Quiet;
        }
    }
    void MoveGen::insertPawnMoves(const Board& board, Move* moves, unsigned short& moveCount, bitboard moveBitboard, short StartSquareDelta, MoveType moveType) {
        MoveType tempMoveType;
        Piece capturedPiece;
        Index square, sourceSquare;

        bitboard promoMoves = moveBitboard & (Bitboard::RANK0 | Bitboard::RANK7);
        bitboard quietMoves = moveBitboard ^ promoMoves;
        Index enPassant = board.getEnPassantTarget();

        Move* out = moves + moveCount;

        while (quietMoves != 0ULL) {
            // get index of least-significant bit and remove the bit. 
            square = Bitboard::popLSB(quietMoves);
            sourceSquare = square - StartSquareDelta;

            if (square == enPassant) {
                *out++ = Move(sourceSquare, square, Piece::PAWN, MoveType::EnPassant, Piece::PAWN);
            }
            else if (moveType == MoveType::Capture) {
                *out++ = Move(sourceSquare, square, Piece::PAWN, moveType, board.getPieceOnSquare(square));
            }
            else {
                *out++ = Move(sourceSquare, square, Piece::PAWN, moveType);
            }
        }

        while (promoMoves != 0ULL) {
            // get index of least-significant bit and remove the bit. 
            square = Bitboard::popLSB(promoMoves);
            sourceSquare = square - StartSquareDelta;

            if (moveType == MoveType::Capture) {
                tempMoveType = MoveType::PromotionCapture;
                capturedPiece = board.getPieceOnSquare(square);
            }
            else {
                tempMoveType = MoveType::Promotion;
                capturedPiece = Piece::NONE;
            }

            *out++ = Move(sourceSquare, square, Piece::PAWN, tempMoveType, capturedPiece, Piece::QUEEN);
            *out++ = Move(sourceSquare, square, Piece::PAWN, tempMoveType, capturedPiece, Piece::ROOK);
            *out++ = Move(sourceSquare, square, Piece::PAWN, tempMoveType, capturedPiece, Piece::BISHOP);
            *out++ = Move(sourceSquare, square, Piece::PAWN, tempMoveType, capturedPiece, Piece::KNIGHT);
        }

        moveCount = out - moves;
    }
    void MoveGen::genLegalPawnMoves(const Board& board, bitboard pawns, Move* moves, unsigned short& moveCount) {
        if (pawns != 0ULL) {
            // get bitboard moves.
            bitboard singlePushes, doublePushes, leftCaptures, rightCaptures;
            MoveGen::genBitboardsLegalPawnMoves(board, pawns, singlePushes, doublePushes, leftCaptures, rightCaptures);
            MoveGen::insertPawnMoves(board, moves, moveCount, singlePushes, board.whiteToMove() ? 8 : -8, MoveType::Quiet);
            MoveGen::insertPawnMoves(board, moves, moveCount, doublePushes, board.whiteToMove() ? 16 : -16, MoveType::DoublePawnPush);
            MoveGen::insertPawnMoves(board, moves, moveCount, leftCaptures, board.whiteToMove() ? 7 : -9, MoveType::Capture);
            MoveGen::insertPawnMoves(board, moves, moveCount, rightCaptures, board.whiteToMove() ? 9 : -7, MoveType::Capture);
        }
    }
    void MoveGen::genLegalHumanMoves(const Board& board, Index square, Move* moves, unsigned short& moveCount) {
        Piece type = board.getPieceOnSquare(square);
        bitboard movesBitboard;

        if (type == Piece::PAWN) {
            MoveGen::genLegalPawnMoves(board, (1ULL << square), moves, moveCount);
        }
        else {
            if (type == Piece::KNIGHT) {
                movesBitboard = MoveGen::genLegalMoves<Piece::KNIGHT>(board, square);
                MoveGen::insertMoves<Piece::KNIGHT>(board, moves, moveCount, square, movesBitboard);
            }
            if (type == Piece::BISHOP) {
                movesBitboard = MoveGen::genLegalMoves<Piece::BISHOP>(board, square);
                MoveGen::insertMoves<Piece::BISHOP>(board, moves, moveCount, square, movesBitboard);
            }
            if (type == Piece::ROOK) {
                movesBitboard = MoveGen::genLegalMoves<Piece::ROOK>(board, square);
                MoveGen::insertMoves<Piece::ROOK>(board, moves, moveCount, square, movesBitboard);
            }
            if (type == Piece::QUEEN) {
                movesBitboard = MoveGen::genLegalMoves<Piece::QUEEN>(board, square);
                MoveGen::insertMoves<Piece::QUEEN>(board, moves, moveCount, square, movesBitboard);
            }
            if (type == Piece::KING) {
                movesBitboard = MoveGen::genLegalMoves<Piece::KING>(board, square);
                MoveGen::insertMoves<Piece::KING>(board, moves, moveCount, square, movesBitboard);
            }
        }
    }
    unsigned short MoveGen::genAllLegalMoves(const Board& board, Move* moves) {
        unsigned short moveCount = 0;

        if (board.numOfChecks() > 1) {
            MoveGen::genLegalHumanMoves(board, board.getKingPos(), moves, moveCount);
            return moveCount;
        }

        // get all pawns.
        bitboard pawns = board.getPieces(Piece::PAWN, board.whiteToMove());

        // insert pawn moves.
        MoveGen::genLegalPawnMoves(board, pawns, moves, moveCount);

        // get all other pieces.
        bitboard nonPawnPieces = board.getFriendlyPieces() & ~pawns;

        Index square;

        // insert all other piece moves.
        while (nonPawnPieces != 0) {
            square = Bitboard::popLSB(nonPawnPieces);

            MoveGen::genLegalHumanMoves(board, square, moves, moveCount);
        }

        return moveCount;
    }
}
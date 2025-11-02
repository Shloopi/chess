#include "MoveGen.hpp"

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
        legalMoves |= (targetSquares & state.checkingPieces);

        // blocking the ray.
        bitboard checkRay = Constants::BETWEEN_PIECES_TABLE[kingSquare][Chess::lsb(state.checkingPieces)];

        legalMoves |= (checkRay & targetSquares);

        return legalMoves;
    }

	template <bool whiteToMove>
    bitboard reducePinnedPiecesMoves(const BoardState& state, Index sourceSquare, bitboard targetSquares) {
        if (!state.isPiecePinned(sourceSquare)) return targetSquares;

        bitboard moveRay, temp = targetSquares, kingBB = Constants::SQUARE_BBS[state.board.getKing<whiteToMove>()];
        Index targetSquare;


        while (temp != 0ULL) {
            targetSquare = Chess::popLSB(temp);
            moveRay = Constants::BETWEEN_TABLE[sourceSquare][targetSquare];

            if ((kingBB & moveRay) == 0ULL) targetSquares &= ~Constants::SQUARE_BBS[targetSquare];
        }

        return targetSquares;
    }

    template uint16_t genAllLegalMoves<true>(const BoardState& state, Move* moves);
    template uint16_t genAllLegalMoves<false>(const BoardState& state, Move* moves);

	template <bool whiteToMove>
    uint16_t genAllLegalMoves(const BoardState& state, Move* moves) {
        uint16_t moveCount = 0;

        if (state.numOfChecks() > 1) {
			MoveGen::genKingMoves<whiteToMove>(state, moves, moveCount);
        }
        else {
			MoveGen::genPawnMoves<whiteToMove>(state, moves, moveCount);
			MoveGen::genKnightMoves<whiteToMove>(state, moves, moveCount);
			MoveGen::genBishopMoves<whiteToMove>(state, moves, moveCount);
			MoveGen::genRookMoves<whiteToMove>(state, moves, moveCount);
			MoveGen::genQueenMoves<whiteToMove>(state, moves, moveCount);
            MoveGen::genKingMoves<whiteToMove>(state, moves, moveCount);
        }
        
        return moveCount;
    }

    template<bool whiteToMove>
    uint16_t countAllLegalMoves(const BoardState& state) {
        uint16_t moveCount = 0;

        if (state.numOfChecks() > 1) {
            MoveGen::genKingMoves<whiteToMove, true>(state, NULL, moveCount);
        }
        else {
            MoveGen::genPawnMoves<whiteToMove, true>(state, NULL, moveCount);
            MoveGen::genKnightMoves<whiteToMove, true>(state, NULL, moveCount);
            MoveGen::genBishopMoves<whiteToMove, true>(state, NULL, moveCount);
            MoveGen::genRookMoves<whiteToMove, true>(state, NULL, moveCount);
            MoveGen::genQueenMoves<whiteToMove, true>(state, NULL, moveCount);
            MoveGen::genKingMoves<whiteToMove, true>(state, NULL, moveCount);
        }

        return moveCount;
    }

    template bool hasLegalMoves<true>(const BoardState& state);
    template bool hasLegalMoves<false>(const BoardState& state);

    template <bool whiteToMove>
    bool hasLegalMoves(const BoardState& state) {
        uint16_t moveCount = 0;

        if (state.numOfChecks() > 1) {
            MoveGen::genKingMoves<whiteToMove, true>(state, NULL, moveCount);
        }
        else {
            MoveGen::genPawnMoves<whiteToMove, true>(state, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genKnightMoves<whiteToMove, true>(state, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genBishopMoves<whiteToMove, true>(state, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genRookMoves<whiteToMove, true>(state, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genQueenMoves<whiteToMove, true>(state, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genKingMoves<whiteToMove, true>(state, NULL, moveCount);
            if (moveCount != 0) return true;
        }

        return moveCount != 0;
    }

    template<bool whiteToMove, bool countOnly>
    void genPawnMoves(const BoardState& state, Move* moves, uint16_t& moveCount) {
		bitboard pawns = state.board.getPawns<whiteToMove>();
		bitboard enemy = state.board.getEnemyPieces<whiteToMove>() | state.board.enPassant;
		bitboard empty = state.board.getFreeSquares();
		Index enPassant = state.board.getEnPassantSquare();
        bitboard checkRay = state.inCheck() ? state.checkingPieces | Constants::BETWEEN_PIECES_TABLE[state.board.getKing<whiteToMove>()][Chess::lsb(state.checkingPieces)] : Chess::MAX_BITBOARD;

		// Pawns that can do pushes.
		bitboard canSinglePush = pawns & Chess::pawnsBackward<whiteToMove>(empty) & ~state.bishopPins;
		bitboard canDoublePush = canSinglePush & Chess::pawnsBackward2<whiteToMove>(empty) & Chess::doublePushRank<whiteToMove>();

		// Pawns that can do left captures.
		bitboard canLeftCapture = pawns & Chess::pawnsRevAttackLeft<whiteToMove>(enemy) & ~state.rookPins & Chess::pawnLeftMask<whiteToMove>();
        
        // Pawns that can do right captures.
        bitboard canRightCapture = pawns & Chess::pawnsRevAttackRight<whiteToMove>(enemy) & ~state.rookPins & Chess::pawnRightMask<whiteToMove>();

        // Remove PinnedPawns.
        Index kingSquare = state.board.getKing<whiteToMove>(), pinnedPawn;
        bitboard pinnedPawnBB, pinRay;

        // Single and double pushes.
        bitboard pinnedPushes = ((canSinglePush | canDoublePush) & state.rookPins);

        while (pinnedPushes != 0) {
            pinnedPawn = Chess::popLSB(pinnedPushes);
            pinnedPawnBB = Constants::SQUARE_BBS[pinnedPawn];

            pinRay = Constants::BETWEEN_TABLE[kingSquare][pinnedPawn];

            if ((pinRay & Chess::pawnsForward<whiteToMove>(pinnedPawnBB)) == 0) {
                canSinglePush &= ~pinnedPawnBB;
            }
            if ((pinRay & Chess::pawnsForward2<whiteToMove>(pinnedPawnBB)) == 0) {
                canDoublePush &= ~pinnedPawnBB;
            }
        }

        // Left and right captures.
        bitboard pinnedCaptures = ((canLeftCapture | canRightCapture) & state.bishopPins);

        while (pinnedCaptures != 0) {
            pinnedPawn = Chess::popLSB(pinnedCaptures);
            pinnedPawnBB = Constants::SQUARE_BBS[pinnedPawn];

            pinRay = Constants::BETWEEN_TABLE[kingSquare][pinnedPawn];

            if ((pinRay & Chess::pawnsAttackLeft<whiteToMove>(pinnedPawnBB & Chess::pawnLeftMask<whiteToMove>())) == 0) {
                canLeftCapture &= ~pinnedPawnBB;
            }
            if ((pinRay & Chess::pawnsAttackRight<whiteToMove>(pinnedPawnBB & Chess::pawnRightMask<whiteToMove>())) == 0) {
                canRightCapture &= ~pinnedPawnBB;
            }
        }

        // Handle En Passant.
        bitboard leftEnPassant, rightEnPassant;
        if (state.board.enPassant != 0 && !state.pinnedEnPassant) {
            leftEnPassant = canLeftCapture & state.board.enPassant;
            canLeftCapture ^= leftEnPassant;

            rightEnPassant = canRightCapture & state.board.enPassant;
            canRightCapture ^= rightEnPassant;
        }

        // Seperate Promotions.
		bitboard promoSinglePush = canSinglePush & Chess::promotionRank<whiteToMove>();
		bitboard promoLeftCapture = canLeftCapture & Chess::promotionRank<whiteToMove>();
		bitboard promoRightCapture = canRightCapture & Chess::promotionRank<whiteToMove>();
		canSinglePush ^= promoSinglePush;
		canLeftCapture ^= promoLeftCapture;
		canRightCapture ^= promoRightCapture;

        // Count Only.
        if constexpr (countOnly) {
            moveCount += Chess::numOfBits(canSinglePush) + Chess::numOfBits(canDoublePush) +
                Chess::numOfBits(canLeftCapture) + Chess::numOfBits(canRightCapture) +
                Chess::numOfBits(leftEnPassant) + Chess::numOfBits(rightEnPassant) +
                4 * (Chess::numOfBits(promoSinglePush) + Chess::numOfBits(promoLeftCapture) +
                    Chess::numOfBits(promoRightCapture));
            return;
        }

		// ----- Insert Moves -----
		Index startSquare, targetSquare;
        bitboard startSquareBB;
		Move* out = moves + moveCount;

		// Insert Single Pushes.
        while (canSinglePush != 0) {
            startSquare = Chess::popLSB(canSinglePush);
            targetSquare = Chess::pawnForward<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::QUIET);
            }
		}

        // Insert Double Pushes.
        while (canDoublePush != 0) {
            startSquare = Chess::popLSB(canDoublePush);
            targetSquare = Chess::pawnForward2<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::DOUBLE_PAWN_PUSH);
            }
        }

        // Insert Left Captures.
        while (canLeftCapture != 0) {
            startSquare = Chess::popLSB(canLeftCapture);
            targetSquare = Chess::pawnAttackLeft<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, targetSquare == enPassant ? Chess::EN_PASSANT : Chess::QUIET);
            }
        }

        // Insert Right Captures.
        while (canRightCapture != 0) {
            startSquare = Chess::popLSB(canRightCapture);
            targetSquare = Chess::pawnAttackRight<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, targetSquare == enPassant ? Chess::EN_PASSANT : Chess::QUIET);
            }
        }

        // Insert Single Pushe Promotions.
        while (promoSinglePush != 0) {
            startSquare = Chess::popLSB(promoSinglePush);
            targetSquare = Chess::pawnForward<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::KNIGHT_PROMOTION);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::BISHOP_PROMOTION);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::ROOK_PROMOTION);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::QUEEN_PROMOTION);
            }
        }

        // Insert Left Capture Promotions.
        while (promoLeftCapture != 0) {
            startSquare = Chess::popLSB(promoLeftCapture);
            targetSquare = Chess::pawnAttackLeft<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::KNIGHT_PROMOTION);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::BISHOP_PROMOTION);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::ROOK_PROMOTION);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::QUEEN_PROMOTION);
            }
        }

        // Insert Right Capture Promotions.
        while (promoRightCapture != 0) {
            startSquare = Chess::popLSB(promoRightCapture);
            targetSquare = Chess::pawnAttackRight<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::KNIGHT_PROMOTION);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::BISHOP_PROMOTION);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::ROOK_PROMOTION);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::QUEEN_PROMOTION);
            }
        }
        moveCount = out - moves;
    }

    template<bool whiteToMove, bool countOnly>
    void genKingMoves(const BoardState& state, Move* moves, uint16_t& moveCount) {
        Index kingSquare = state.board.getKing<whiteToMove>();
        bitboard pieces = state.board.getAllPieces();
        bitboard noKingPieces = pieces & ~Constants::SQUARE_BBS[kingSquare];
        bitboard moveBitboard = PseudoMoveGen::getPseudoKingMoves(state.board.notFriendlyPieces<whiteToMove>(), kingSquare);
        bitboard tempBitboard = moveBitboard;
        Index targetSquare, targetCastlingSquare;

        // Insertion.
        Move* out = moves + moveCount;

        // run for every move and check if the target square is attacked.
        while (tempBitboard != 0) {
            // get the index of the target square and remove it from the bitboard.
            targetSquare = Chess::popLSB(tempBitboard);

            // if the target square is attacked, remove it from the moves bitboard.
            if (!state.isSquareAttacked<whiteToMove>(targetSquare, noKingPieces)) {
                if constexpr (countOnly) moveCount++;
                else *out++ = Move(kingSquare, targetSquare, Chess::KING, Chess::REMOVE_ALL_CASTLING);
            }
        }

        // check for kingside castling.
        targetCastlingSquare = kingSquare + 2;
        if (!state.inCheck() && state.board.canCastleShort<whiteToMove>() && (moveBitboard & (1ULL << (kingSquare + 1))) != 0 && (pieces & (1ULL << targetCastlingSquare)) == 0 && (pieces & (1ULL << (kingSquare + 1))) == 0 && !state.isSquareAttacked<whiteToMove>(targetCastlingSquare, state.board.getAllPieces())) {
            if constexpr (countOnly) moveCount++;
            else *out++ = Move(kingSquare, targetCastlingSquare, Chess::KING, Chess::SHORT_CASTLING);
        }

        // check for queenside castling.
        targetCastlingSquare = kingSquare - 2;
        if (!state.inCheck() && state.board.canCastleLong<whiteToMove>() && (moveBitboard & (1ULL << (kingSquare - 1))) != 0 && (pieces & (1ULL << targetCastlingSquare)) == 0 && (pieces & (1ULL << (kingSquare - 1))) == 0 && !state.isSquareAttacked<whiteToMove>(targetCastlingSquare, state.board.getAllPieces())) {
            if constexpr (countOnly) moveCount++;
            else *out++ = Move(kingSquare, targetCastlingSquare, Chess::KING, Chess::LONG_CASTLING);
        }

        if constexpr (!countOnly) moveCount = out - moves;
    }
    template <bool whiteToMove, bool countOnly>
    void genKnightMoves(const BoardState& state, Move* moves, uint16_t& moveCount) {
        Index startSquare, targetSquare;
        bitboard movesBitboard = 0ULL, enemyOrEmpty = state.board.notFriendlyPieces<whiteToMove>();
		bitboard knights = state.board.getKnights<whiteToMove>();
		Move* out = moves + moveCount;
        
		// all pinned knights cannot move.
        knights &= ~state.getPinnedPieces();

        while (knights != 0) {
            startSquare = Chess::popLSB(knights);
			movesBitboard = PseudoMoveGen::getPseudoKnightMoves(enemyOrEmpty, startSquare);
            movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(state, startSquare, movesBitboard);

            if constexpr (countOnly) moveCount += Chess::numOfBits(movesBitboard);
            else {
                while (movesBitboard != 0ULL) {
                    targetSquare = Chess::popLSB(movesBitboard);
                    *out++ = Move(startSquare, targetSquare, Chess::KNIGHT, Chess::QUIET);
                }
            }
        }
        if constexpr (!countOnly) moveCount = out - moves;
    }

    template <bool whiteToMove, bool countOnly>
    void genBishopMoves(const BoardState& state, Move* moves, uint16_t& moveCount) {
        Index startSquare, targetSquare;
        bitboard movesBitboard = 0ULL, enemyOrEmpty = state.board.notFriendlyPieces<whiteToMove>();
        bitboard allPieces = state.board.getAllPieces(), bishops = state.board.getBishops<whiteToMove>();
        Move* out = moves + moveCount;

		// all pinned bishops by rooks cannot move.
		bishops &= ~state.rookPins;

        while (bishops != 0) {
            startSquare = Chess::popLSB(bishops);
            movesBitboard = PseudoMoveGen::getPseudoBishopMoves(enemyOrEmpty, startSquare, allPieces);
            movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(state, startSquare, movesBitboard);
            movesBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(state, startSquare, movesBitboard);

            if constexpr (countOnly) moveCount += Chess::numOfBits(movesBitboard);
            else {
                while (movesBitboard != 0ULL) {
                    targetSquare = Chess::popLSB(movesBitboard);
                    *out++ = Move(startSquare, targetSquare, Chess::BISHOP, Chess::QUIET);
                }
            }
        }
        if constexpr (!countOnly) moveCount = out - moves;
    }

    template <bool whiteToMove, bool countOnly>
    void genQueenMoves(const BoardState& state, Move* moves, uint16_t& moveCount) {
        Index startSquare, targetSquare;
        bitboard movesBitboard = 0ULL, enemyOrEmpty = state.board.notFriendlyPieces<whiteToMove>();
		bitboard allPieces = state.board.getAllPieces(),  queens = state.board.getQueens<whiteToMove>();
        Move* out = moves + moveCount;

        while (queens != 0) {
            startSquare = Chess::popLSB(queens);
            movesBitboard = PseudoMoveGen::getPseudoQueenMoves(enemyOrEmpty, startSquare, allPieces);
            movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(state, startSquare, movesBitboard);
            movesBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(state, startSquare, movesBitboard);

            if constexpr (countOnly) moveCount += Chess::numOfBits(movesBitboard);
            else {
                while (movesBitboard != 0ULL) {
                    targetSquare = Chess::popLSB(movesBitboard);
                    *out++ = Move(startSquare, targetSquare, Chess::QUEEN, Chess::QUIET);
                }
            }
        }
        if constexpr (!countOnly) moveCount = out - moves;
    }

    template <bool whiteToMove, bool countOnly>
    void genRookMoves(const BoardState& state, Move* moves, uint16_t& moveCount) {
        Flag flag;
        Index startSquare, targetSquare;
        bitboard startSquareBB, movesBitboard = 0ULL, enemyOrEmpty = state.board.notFriendlyPieces<whiteToMove>();
        bitboard allPieces = state.board.getAllPieces(), rooks = state.board.getRooks<whiteToMove>();
        Move* out = moves + moveCount;

        // all pinned rooks by bishops cannot move.
        rooks &= ~state.bishopPins;

        while (rooks != 0) {
            startSquare = Chess::popLSB(rooks);
			startSquareBB = Constants::SQUARE_BBS[startSquare];

            movesBitboard = PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, startSquare, allPieces);
            movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(state, startSquare, movesBitboard);
            movesBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(state, startSquare, movesBitboard);

            if constexpr (countOnly) moveCount += Chess::numOfBits(movesBitboard);
            else {
                while (movesBitboard != 0ULL) {
                    targetSquare = Chess::popLSB(movesBitboard);

                    if ((startSquareBB & Board::startingKingsideRook<whiteToMove>()) != 0) flag = Chess::REMOVE_SHORT_CASTLING;
                    else if ((startSquareBB & Board::startingQueensideRook<whiteToMove>()) != 0) flag = Chess::REMOVE_LONG_CASTLING;
                    else flag = Chess::QUIET;

                    *out++ = Move(startSquare, targetSquare, Chess::ROOK, flag);
                }
            }
        }
        if constexpr (!countOnly) moveCount = out - moves;
    }
}
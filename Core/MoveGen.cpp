#include "MoveGen.hpp"
#include "Game.hpp"

namespace MoveGen {
    template <bool whiteToMove>
    bitboard movesLegalityWhileChecked(const Game& game, Index sourceSquare, bitboard targetSquares) {
        // if there is no check, return all moves.
        if (!game.state.inCheck()) return targetSquares;

        // if there are 2 checks, only king moves are legal.
        if (game.state.numOfChecks() > 1) return 0ULL;

        // calculate if moves are legal when there is one checking piece.
        bitboard legalMoves = 0ULL;
        Index kingSquare = game.board.getKing<whiteToMove>();

        // capturing the enemy checking piece.
        legalMoves |= (targetSquares & game.state.checkingPieces);

        // blocking the ray.
        bitboard checkRay = Constants::BETWEEN_PIECES_TABLE[kingSquare][Chess::lsb(game.state.checkingPieces)];

        legalMoves |= (checkRay & targetSquares);

        return legalMoves;
    }

	template <bool whiteToMove>
    bitboard reducePinnedPiecesMoves(const Game& game, Index sourceSquare, bitboard targetSquares) {
        if (!game.state.isPiecePinned(sourceSquare)) return targetSquares;

        bitboard moveRay, temp = targetSquares, kingBB = Constants::SQUARE_BBS[game.board.getKing<whiteToMove>()];
        Index targetSquare;


        while (temp != 0ULL) {
            targetSquare = Chess::popLSB(temp);
            moveRay = Constants::BETWEEN_TABLE[sourceSquare][targetSquare];

            if ((kingBB & moveRay) == 0ULL) targetSquares &= ~Constants::SQUARE_BBS[targetSquare];
        }

        return targetSquares;
    }

    template uint8_t genAllLegalMoves<true>(const Game& game, Move* moves);
    template uint8_t genAllLegalMoves<false>(const Game& game, Move* moves);

	template <bool whiteToMove>
    uint8_t genAllLegalMoves(const Game& game, Move* moves) {
        uint8_t moveCount = 0;

        if (game.state.numOfChecks() > 1) {
			MoveGen::genKingMoves<whiteToMove>(game, moves, moveCount);
        }
        else {
			MoveGen::genPawnMoves<whiteToMove>(game, moves, moveCount);
			MoveGen::genKnightMoves<whiteToMove>(game, moves, moveCount);
			MoveGen::genBishopMoves<whiteToMove>(game, moves, moveCount);
			MoveGen::genRookMoves<whiteToMove>(game, moves, moveCount);
			MoveGen::genQueenMoves<whiteToMove>(game, moves, moveCount);
            MoveGen::genKingMoves<whiteToMove>(game, moves, moveCount);
        }
        
        return moveCount;
    }

    template<bool whiteToMove>
    uint8_t countAllLegalMoves(const Game& game) {
        uint8_t moveCount = 0;

        if (game.state.numOfChecks() > 1) {
            MoveGen::genKingMoves<whiteToMove, true>(game, NULL, moveCount);
        }
        else {
            MoveGen::genPawnMoves<whiteToMove, true>(game, NULL, moveCount);
            MoveGen::genKnightMoves<whiteToMove, true>(game, NULL, moveCount);
            MoveGen::genBishopMoves<whiteToMove, true>(game, NULL, moveCount);
            MoveGen::genRookMoves<whiteToMove, true>(game, NULL, moveCount);
            MoveGen::genQueenMoves<whiteToMove, true>(game, NULL, moveCount);
            MoveGen::genKingMoves<whiteToMove, true>(game, NULL, moveCount);
        }

        return moveCount;
    }

    template bool hasLegalMoves<true>(const Game& game);
    template bool hasLegalMoves<false>(const Game& game);

    template <bool whiteToMove>
    bool hasLegalMoves(const Game& game) {
        uint8_t moveCount = 0;

        if (game.state.numOfChecks() > 1) {
            MoveGen::genKingMoves<whiteToMove, true>(game, NULL, moveCount);
        }
        else {
            MoveGen::genPawnMoves<whiteToMove, true>(game, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genKnightMoves<whiteToMove, true>(game, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genBishopMoves<whiteToMove, true>(game, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genRookMoves<whiteToMove, true>(game, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genQueenMoves<whiteToMove, true>(game, NULL, moveCount);
            if (moveCount != 0) return true;
            MoveGen::genKingMoves<whiteToMove, true>(game, NULL, moveCount);
            if (moveCount != 0) return true;
        }

        return moveCount != 0;
    }

    template<bool whiteToMove, bool countOnly>
    void genPawnMoves(const Game& game, Move* moves, uint8_t& moveCount) {
		bitboard pawns = game.board.getPawns<whiteToMove>();

		bitboard enemy = game.board.getEnemyPieces<whiteToMove>();
		if (!game.state.pinnedEnPassant) enemy |= game.board.enPassant;

		bitboard empty = game.board.getFreeSquares();
		Index enPassant = game.board.getEnPassantSquare();
        bitboard checkRay = game.state.inCheck() ? game.state.checkingPieces | Constants::BETWEEN_PIECES_TABLE[game.board.getKing<whiteToMove>()][Chess::lsb(game.state.checkingPieces)] : Chess::MAX_BITBOARD;

		// Pawns that can do pushes.
		bitboard canSinglePush = pawns & Chess::pawnsBackward<whiteToMove>(empty) & ~game.state.bishopPins;
		bitboard canDoublePush = canSinglePush & Chess::pawnsBackward2<whiteToMove>(empty) & Chess::doublePushRank<whiteToMove>();

		// Pawns that can do left captures.
		bitboard canLeftCapture = pawns & Chess::pawnsRevAttackLeft<whiteToMove>(enemy) & ~game.state.rookPins & Chess::pawnLeftMask<whiteToMove>();
        
        // Pawns that can do right captures.
        bitboard canRightCapture = pawns & Chess::pawnsRevAttackRight<whiteToMove>(enemy) & ~game.state.rookPins & Chess::pawnRightMask<whiteToMove>();

        // Remove PinnedPawns.
        Index kingSquare = game.board.getKing<whiteToMove>(), pinnedPawn;
        bitboard pinnedPawnBB, pinRay;

        // Single and double pushes.
        bitboard pinnedPushes = ((canSinglePush | canDoublePush) & game.state.rookPins);

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
        bitboard pinnedCaptures = ((canLeftCapture | canRightCapture) & game.state.bishopPins);

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
                4 * (Chess::numOfBits(promoSinglePush) + Chess::numOfBits(promoLeftCapture) +
                    Chess::numOfBits(promoRightCapture));
            return;
        }

		// ----- Insert Moves -----
		Index startSquare, targetSquare;
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
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, targetSquare == enPassant ? Chess::EN_PASSANT : Chess::QUIET, true);
            }
        }

        // Insert Right Captures.
        while (canRightCapture != 0) {
            startSquare = Chess::popLSB(canRightCapture);
            targetSquare = Chess::pawnAttackRight<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, targetSquare == enPassant ? Chess::EN_PASSANT : Chess::QUIET, true);
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
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::KNIGHT_PROMOTION, true);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::BISHOP_PROMOTION, true);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::ROOK_PROMOTION, true);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::QUEEN_PROMOTION, true);
            }
        }

        // Insert Right Capture Promotions.
        while (promoRightCapture != 0) {
            startSquare = Chess::popLSB(promoRightCapture);
            targetSquare = Chess::pawnAttackRight<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::KNIGHT_PROMOTION, true);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::BISHOP_PROMOTION, true);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::ROOK_PROMOTION, true);
                *out++ = Move(startSquare, targetSquare, Chess::PAWN, Chess::QUEEN_PROMOTION, true);
            }
        }
        moveCount = out - moves;
    }

    template<bool whiteToMove, bool countOnly>
    void genKingMoves(const Game& game, Move* moves, uint8_t& moveCount) {
        Index kingSquare = game.board.getKing<whiteToMove>();
        bitboard pieces = game.board.getAllPieces();
		bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard noKingPieces = pieces & ~Constants::SQUARE_BBS[kingSquare];
        bitboard moveBitboard = PseudoMoveGen::getPseudoKingMoves(game.board.notFriendlyPieces<whiteToMove>(), kingSquare);
        bitboard tempBitboard = moveBitboard;
        Index targetSquare, targetCastlingSquare;

        // Insertion.
        Move* out = moves + moveCount;

        // run for every move and check if the target square is attacked.
        while (tempBitboard != 0) {
            // get the index of the target square and remove it from the bitboard.
            targetSquare = Chess::popLSB(tempBitboard);

            // if the target square is attacked, remove it from the moves bitboard.
            if (!game.board.isSquareAttacked<whiteToMove>(targetSquare, noKingPieces)) {
                if constexpr (countOnly) moveCount++;
                else {
                    *out++ = Move(kingSquare, targetSquare, Chess::KING, Chess::REMOVE_ALL_CASTLING, Constants::SQUARE_BBS[targetSquare] & enemyPieces);
                }
            }
            else {
				moveBitboard &= ~Constants::SQUARE_BBS[targetSquare];
            }
        }

        // check for kingside castling.
        targetCastlingSquare = kingSquare + 2;
        if (!game.state.inCheck() && 
            game.board.canCastleShort<whiteToMove>() && 
            (moveBitboard & Constants::SQUARE_BBS[kingSquare + 1]) != 0 &&
            (pieces & Constants::SQUARE_BBS[targetCastlingSquare]) == 0 && 
            (pieces & Constants::SQUARE_BBS[kingSquare + 1]) == 0 && 
            !game.board.isSquareAttacked<whiteToMove>(targetCastlingSquare, pieces)) {
            
                if constexpr (countOnly) moveCount++;
                else *out++ = Move(kingSquare, targetCastlingSquare, Chess::KING, Chess::SHORT_CASTLING);
        }

        // check for queenside castling.
        targetCastlingSquare = kingSquare - 2;
        if (!game.state.inCheck() && 
            game.board.canCastleLong<whiteToMove>() && 
            (moveBitboard & Constants::SQUARE_BBS[kingSquare - 1]) != 0 &&
            (pieces & Constants::SQUARE_BBS[targetCastlingSquare]) == 0 && 
            (pieces & Constants::SQUARE_BBS[targetCastlingSquare - 1]) == 0 && 
            (pieces & Constants::SQUARE_BBS[kingSquare - 1]) == 0 && 
            !game.board.isSquareAttacked<whiteToMove>(targetCastlingSquare, pieces)) {
            
                if constexpr (countOnly) moveCount++;
                else *out++ = Move(kingSquare, targetCastlingSquare, Chess::KING, Chess::LONG_CASTLING);
        }

        if constexpr (!countOnly) moveCount = out - moves;
    }
    template <bool whiteToMove, bool countOnly>
    void genKnightMoves(const Game& game, Move* moves, uint8_t& moveCount) {
        Index startSquare, targetSquare;
        bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard movesBitboard = 0ULL, enemyOrEmpty = game.board.notFriendlyPieces<whiteToMove>();
		bitboard knights = game.board.getKnights<whiteToMove>();
		Move* out = moves + moveCount;
        
		// all pinned knights cannot move.
        knights &= ~game.state.getPinnedPieces();

        while (knights != 0) {
            startSquare = Chess::popLSB(knights);
			movesBitboard = PseudoMoveGen::getPseudoKnightMoves(enemyOrEmpty, startSquare);
            movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(game, startSquare, movesBitboard);

            if constexpr (countOnly) moveCount += Chess::numOfBits(movesBitboard);
            else {
                while (movesBitboard != 0ULL) {
                    targetSquare = Chess::popLSB(movesBitboard);
                    *out++ = Move(startSquare, targetSquare, Chess::KNIGHT, Chess::QUIET, Constants::SQUARE_BBS[targetSquare] & enemyPieces);
                }
            }
        }
        if constexpr (!countOnly) moveCount = out - moves;
    }

    template <bool whiteToMove, bool countOnly>
    void genBishopMoves(const Game& game, Move* moves, uint8_t& moveCount) {
        Index startSquare, targetSquare;
        bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard movesBitboard = 0ULL, enemyOrEmpty = game.board.notFriendlyPieces<whiteToMove>();
        bitboard allPieces = game.board.getAllPieces(), bishops = game.board.getBishops<whiteToMove>();
        Move* out = moves + moveCount;

		// all pinned bishops by rooks cannot move.
		bishops &= ~game.state.rookPins;

        while (bishops != 0) {
            startSquare = Chess::popLSB(bishops);
            movesBitboard = PseudoMoveGen::getPseudoBishopMoves(enemyOrEmpty, startSquare, allPieces);
            movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(game, startSquare, movesBitboard);
            movesBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(game, startSquare, movesBitboard);

            if constexpr (countOnly) moveCount += Chess::numOfBits(movesBitboard);
            else {
                while (movesBitboard != 0ULL) {
                    targetSquare = Chess::popLSB(movesBitboard);
                    *out++ = Move(startSquare, targetSquare, Chess::BISHOP, Chess::QUIET, Constants::SQUARE_BBS[targetSquare] & enemyPieces);
                }
            }
        }
        if constexpr (!countOnly) moveCount = out - moves;
    }

    template <bool whiteToMove, bool countOnly>
    void genQueenMoves(const Game& game, Move* moves, uint8_t& moveCount) {
        Index startSquare, targetSquare;
        bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard movesBitboard = 0ULL, enemyOrEmpty = game.board.notFriendlyPieces<whiteToMove>();
		bitboard allPieces = game.board.getAllPieces(),  queens = game.board.getQueens<whiteToMove>();
        Move* out = moves + moveCount;

        while (queens != 0) {
            startSquare = Chess::popLSB(queens);
            movesBitboard = PseudoMoveGen::getPseudoQueenMoves(enemyOrEmpty, startSquare, allPieces);
            movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(game, startSquare, movesBitboard);
            movesBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(game, startSquare, movesBitboard);

            if constexpr (countOnly) moveCount += Chess::numOfBits(movesBitboard);
            else {
                while (movesBitboard != 0ULL) {
                    targetSquare = Chess::popLSB(movesBitboard);
                    *out++ = Move(startSquare, targetSquare, Chess::QUEEN, Chess::QUIET, Constants::SQUARE_BBS[targetSquare] & enemyPieces);
                }
            }
        }
        if constexpr (!countOnly) moveCount = out - moves;
    }

    template <bool whiteToMove, bool countOnly>
    void genRookMoves(const Game& game, Move* moves, uint8_t& moveCount) {
        Flag flag;
        Index startSquare, targetSquare;
        bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard startSquareBB, movesBitboard = 0ULL, enemyOrEmpty = game.board.notFriendlyPieces<whiteToMove>();
        bitboard allPieces = game.board.getAllPieces(), rooks = game.board.getRooks<whiteToMove>();
        Move* out = moves + moveCount;

        // all pinned rooks by bishops cannot move.
        rooks &= ~game.state.bishopPins;

        while (rooks != 0) {
            startSquare = Chess::popLSB(rooks);
			startSquareBB = Constants::SQUARE_BBS[startSquare];

            movesBitboard = PseudoMoveGen::getPseudoRookMoves(enemyOrEmpty, startSquare, allPieces);
            movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(game, startSquare, movesBitboard);
            movesBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(game, startSquare, movesBitboard);

            if constexpr (countOnly) moveCount += Chess::numOfBits(movesBitboard);
            else {
                while (movesBitboard != 0ULL) {
                    targetSquare = Chess::popLSB(movesBitboard);

                    if ((startSquareBB & Board::startingKingsideRook<whiteToMove>()) != 0) flag = Chess::REMOVE_SHORT_CASTLING;
                    else if ((startSquareBB & Board::startingQueensideRook<whiteToMove>()) != 0) flag = Chess::REMOVE_LONG_CASTLING;
                    else flag = Chess::QUIET;

                    *out++ = Move(startSquare, targetSquare, Chess::ROOK, flag, Constants::SQUARE_BBS[targetSquare] & enemyPieces);
                }
            }
        }
        if constexpr (!countOnly) moveCount = out - moves;
    }
}
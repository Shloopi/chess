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
    
    template bitboard getAttackingSquares<Chess::NO_PIECE>(const Game& game, bool color);
    template bitboard getAttackingSquares<Chess::PAWN>(const Game& game, bool color);
    template bitboard getAttackingSquares<Chess::KNIGHT>(const Game& game, bool color);
    template bitboard getAttackingSquares<Chess::BISHOP>(const Game& game, bool color);
    template bitboard getAttackingSquares<Chess::ROOK>(const Game& game, bool color);
    template bitboard getAttackingSquares<Chess::QUEEN>(const Game& game, bool color);
    template bitboard getAttackingSquares<Chess::KING>(const Game& game, bool color);

    template <Piece piece>
    bitboard getAttackingSquares(const Game& game, bool color) {
        if (color) return getAttackingSquares<true, piece>(game);
        else return getAttackingSquares<false, piece>(game);
    }

    template<bool whiteToMove, Piece piece>
    bitboard getAttackingSquares(const Game& game)
    {
        bitboard allPieces = game.board.getAllPieces();
        Index kingSquare = game.board.getKing<whiteToMove>();
        bitboard leftAttack = 0, rightAttack = 0, knightAttacks = 0,
            bishopAttacks = 0, rookAttacks = 0, queenAttacks = 0, kingAttacks = 0;
        // Pawns
        if constexpr (piece == Chess::NO_PIECE || piece == Chess::PAWN) {
            bitboard pawns = game.board.getPawns<whiteToMove>() & ~game.state.rookPins;
            leftAttack = pawns & Chess::pawnLeftMask<whiteToMove>();
            rightAttack = pawns & Chess::pawnRightMask<whiteToMove>();
            
            bitboard pinRay, pinnedPawnBB, diagonalPinnedPawns = pawns & game.state.bishopPins;
            Index pinnedPawn;
            while (diagonalPinnedPawns != 0) {
                pinnedPawn = Chess::popLSB(diagonalPinnedPawns);
                pinnedPawnBB = Constants::SQUARE_BBS[pinnedPawn];

                pinRay = Constants::BETWEEN_TABLE[kingSquare][pinnedPawn];

                if ((pinRay & Chess::pawnsAttackLeft<whiteToMove>(pinnedPawnBB & Chess::pawnLeftMask<whiteToMove>())) == 0) {
                    leftAttack &= ~pinnedPawnBB;
                }
                if ((pinRay & Chess::pawnsAttackRight<whiteToMove>(pinnedPawnBB & Chess::pawnRightMask<whiteToMove>())) == 0) {
                    rightAttack &= ~pinnedPawnBB;
                }
            }

            leftAttack = Chess::pawnsAttackLeft<whiteToMove>(leftAttack);
            rightAttack = Chess::pawnsAttackRight<whiteToMove>(rightAttack);

            if constexpr (piece == Chess::PAWN) return leftAttack | rightAttack;
        }

        // Knights
        if constexpr (piece == Chess::NO_PIECE || piece == Chess::KNIGHT) {
            bitboard knights = game.board.getKnights<whiteToMove>() & ~game.state.getPinnedPieces();
            Index knightSquare;
            while (knights != 0) {
                knightSquare = Chess::popLSB(knights);

                knightAttacks |= PseudoMoveGen::getPseudoKnightMoves(Chess::MAX_BITBOARD, knightSquare);
            }

            if constexpr (piece == Chess::KNIGHT) return knightAttacks;
        }

        // Bishops
        if constexpr (piece == Chess::NO_PIECE || piece == Chess::BISHOP) {
            bitboard bishops = game.board.getBishops<whiteToMove>() & ~game.state.rookPins, bishopAttackBitboard;

            Index startSquare;
            while (bishops != 0) {
                startSquare = Chess::popLSB(bishops);
                bishopAttackBitboard = PseudoMoveGen::getPseudoBishopMoves(Chess::MAX_BITBOARD, startSquare, allPieces);
                bishopAttackBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(game, startSquare, bishopAttackBitboard);
                bishopAttacks |= bishopAttackBitboard;
            }

            if constexpr (piece == Chess::BISHOP) return bishopAttacks;
        }
        // Rooks
        if constexpr (piece == Chess::NO_PIECE || piece == Chess::ROOK) {
            bitboard rooks = game.board.getRooks<whiteToMove>() & ~game.state.bishopPins, rookAttackBitboard;

            Index startSquare;
            while (rooks != 0) {
                startSquare = Chess::popLSB(rooks);
                rookAttackBitboard = PseudoMoveGen::getPseudoRookMoves(Chess::MAX_BITBOARD, startSquare, allPieces);
                rookAttackBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(game, startSquare, rookAttackBitboard);
                rookAttacks |= rookAttackBitboard;
            }

            if constexpr (piece == Chess::ROOK) return rookAttacks;
        }

        // Queens
        if constexpr (piece == Chess::NO_PIECE || piece == Chess::QUEEN) {
            bitboard queens = game.board.getQueens<whiteToMove>(), queenAttackBitboard;

            Index startSquare;
            while (queens != 0) {
                startSquare = Chess::popLSB(queens);
                queenAttackBitboard = PseudoMoveGen::getPseudoQueenMoves(Chess::MAX_BITBOARD, startSquare, allPieces);
                queenAttackBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(game, startSquare, queenAttackBitboard);
                queenAttacks |= queenAttackBitboard;
            }

            if constexpr (piece == Chess::QUEEN) return queenAttacks;
        }

        // King
        if constexpr (piece == Chess::NO_PIECE || piece == Chess::KING) {
            bitboard noKingPieces = allPieces & ~Constants::SQUARE_BBS[kingSquare];
            kingAttacks = PseudoMoveGen::getPseudoKingMoves(Chess::MAX_BITBOARD, kingSquare);
            bitboard tempBitboard = kingAttacks;
            Index targetSquare;
            uint8_t moveCount = 0;

            while (tempBitboard != 0) {
                targetSquare = Chess::popLSB(tempBitboard);

                if (game.board.isSquareAttacked<whiteToMove>(targetSquare, noKingPieces)) {
                    kingAttacks &= ~Constants::SQUARE_BBS[targetSquare];
                }
            }

            if constexpr (piece == Chess::KING) return kingAttacks;
        }

        if constexpr (piece == Chess::NO_PIECE) {
            return leftAttack | rightAttack | knightAttacks |
                bishopAttacks | rookAttacks | queenAttacks | kingAttacks;
        }
    }

    void genAllLegalMoves(const Game& game, Moves<>& moves) {
        if (game.whiteToMove) genAllLegalMoves<true>(game, moves);
        else genAllLegalMoves<false>(game, moves);
    }

    template void genAllLegalMoves<true>(const Game& game, Moves<>& moves);
    template void genAllLegalMoves<false>(const Game& game, Moves<>& moves);

    template <bool whiteToMove>
    void genAllLegalMoves(const Game& game, Moves<>& moves) {

        if (game.state.numOfChecks() > 1) {
			MoveGen::genKingMoves<whiteToMove>(game, &moves);
        }
        else {
			MoveGen::genPawnMoves<whiteToMove>(game, &moves);
			MoveGen::genKnightMoves<whiteToMove>(game, &moves);
			MoveGen::genBishopMoves<whiteToMove>(game, &moves);
			MoveGen::genRookMoves<whiteToMove>(game, &moves);
			MoveGen::genQueenMoves<whiteToMove>(game, &moves);
            MoveGen::genKingMoves<whiteToMove>(game, &moves);
        }
    }

    template<bool whiteToMove>
    uint8_t countAllLegalMoves(const Game& game) {
        uint8_t moveCount = 0;

        if (game.state.numOfChecks() > 1) {
            moveCount += MoveGen::genKingMoves<whiteToMove, true>(game);
        }
        else {
            moveCount += MoveGen::genPawnMoves<whiteToMove, true>(game);
            moveCount += MoveGen::genKnightMoves<whiteToMove, true>(game);
            moveCount += MoveGen::genBishopMoves<whiteToMove, true>(game);
            moveCount += MoveGen::genRookMoves<whiteToMove, true>(game);
            moveCount += MoveGen::genQueenMoves<whiteToMove, true>(game);
            moveCount += MoveGen::genKingMoves<whiteToMove, true>(game);
        }

        return moveCount;
    }

    bool hasLegalMoves(const Game& game, bool whiteToMove) {
        if (game.whiteToMove) return hasLegalMoves<true>(game);
        else return hasLegalMoves<false>(game);
    }

    template bool hasLegalMoves<true>(const Game& game);
    template bool hasLegalMoves<false>(const Game& game);

    template <bool whiteToMove>
    bool hasLegalMoves(const Game& game) {
        uint8_t moveCount = 0;

        if (game.state.numOfChecks() > 1) {
            moveCount += MoveGen::genKingMoves<whiteToMove, true>(game);
        }
        else {
            moveCount += MoveGen::genPawnMoves<whiteToMove, true>(game);
            if (moveCount != 0) return true;
            moveCount += MoveGen::genKnightMoves<whiteToMove, true>(game);
            if (moveCount != 0) return true;
            moveCount += MoveGen::genBishopMoves<whiteToMove, true>(game);
            if (moveCount != 0) return true;
            moveCount += MoveGen::genRookMoves<whiteToMove, true>(game);
            if (moveCount != 0) return true;
            moveCount += MoveGen::genQueenMoves<whiteToMove, true>(game);
            if (moveCount != 0) return true;
            moveCount += MoveGen::genKingMoves<whiteToMove, true>(game);
            if (moveCount != 0) return true;
        }

        return moveCount != 0;
    }

    template<bool whiteToMove, bool countOnly>
    uint8_t genPawnMoves(const Game& game, Moves<>* moves) {
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
            return Chess::numOfBits(Chess::pawnsForward<whiteToMove>(canSinglePush) & checkRay) +
                Chess::numOfBits(Chess::pawnsForward2<whiteToMove>(canDoublePush) & checkRay) +
                Chess::numOfBits(Chess::pawnsAttackLeft<whiteToMove>(canLeftCapture) & checkRay) +
                Chess::numOfBits(Chess::pawnsAttackRight<whiteToMove>(canRightCapture) & checkRay) +
                4 * (Chess::numOfBits(Chess::pawnsForward<whiteToMove>(promoSinglePush) & checkRay) + 
                    Chess::numOfBits(Chess::pawnsAttackLeft<whiteToMove>(promoLeftCapture) & checkRay) +
                    Chess::numOfBits(Chess::pawnsAttackRight<whiteToMove>(promoRightCapture) & checkRay));
        }

		// ----- Insert Moves -----
		Index startSquare, targetSquare;

		// Insert Single Pushes.
        while (canSinglePush != 0) {
            startSquare = Chess::popLSB(canSinglePush);
            targetSquare = Chess::pawnForward<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::QUIET));
            }
		}

        // Insert Double Pushes.
        while (canDoublePush != 0) {
            startSquare = Chess::popLSB(canDoublePush);
            targetSquare = Chess::pawnForward2<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::DOUBLE_PAWN_PUSH));

            }
        }

        // Insert Left Captures.
        while (canLeftCapture != 0) {
            startSquare = Chess::popLSB(canLeftCapture);
            targetSquare = Chess::pawnAttackLeft<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, targetSquare == enPassant ? Chess::EN_PASSANT : Chess::QUIET, true));
            }
        }

        // Insert Right Captures.
        while (canRightCapture != 0) {
            startSquare = Chess::popLSB(canRightCapture);
            targetSquare = Chess::pawnAttackRight<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, targetSquare == enPassant ? Chess::EN_PASSANT : Chess::QUIET, true));
            }
        }

        // Insert Single Pushe Promotions.
        while (promoSinglePush != 0) {
            startSquare = Chess::popLSB(promoSinglePush);
            targetSquare = Chess::pawnForward<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::KNIGHT_PROMOTION));
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::BISHOP_PROMOTION));
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::ROOK_PROMOTION));
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::QUEEN_PROMOTION));
            }
        }

        // Insert Left Capture Promotions.
        while (promoLeftCapture != 0) {
            startSquare = Chess::popLSB(promoLeftCapture);
            targetSquare = Chess::pawnAttackLeft<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::KNIGHT_PROMOTION, true));
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::BISHOP_PROMOTION, true));
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::ROOK_PROMOTION, true));
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::QUEEN_PROMOTION, true));
            }
        }

        // Insert Right Capture Promotions.
        while (promoRightCapture != 0) {
            startSquare = Chess::popLSB(promoRightCapture);
            targetSquare = Chess::pawnAttackRight<whiteToMove>(startSquare);

            if ((Constants::SQUARE_BBS[targetSquare] & checkRay) != 0ULL) {
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::KNIGHT_PROMOTION, true));
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::BISHOP_PROMOTION, true));
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::ROOK_PROMOTION, true));
                moves->addMove(Move(startSquare, targetSquare, Chess::PAWN, Chess::QUEEN_PROMOTION, true));
            }
        }

        return 0;
    }

    template<bool whiteToMove, bool countOnly>
    uint8_t genKingMoves(const Game& game, Moves<>* moves) {
        Index kingSquare = game.board.getKing<whiteToMove>();
        bitboard pieces = game.board.getAllPieces();
		bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard noKingPieces = pieces & ~Constants::SQUARE_BBS[kingSquare];
        bitboard moveBitboard = PseudoMoveGen::getPseudoKingMoves(game.board.notFriendlyPieces<whiteToMove>(), kingSquare);
        bitboard tempBitboard = moveBitboard;
        Index targetSquare, targetCastlingSquare;
		uint8_t moveCount = 0;

        // run for every move and check if the target square is attacked.
        while (tempBitboard != 0) {
            // get the index of the target square and remove it from the bitboard.
            targetSquare = Chess::popLSB(tempBitboard);

            // if the target square is attacked, remove it from the moves bitboard.
            if (!game.board.isSquareAttacked<whiteToMove>(targetSquare, noKingPieces)) {
                if constexpr (countOnly) moveCount++;
                else {
                    moves->addMove(Move(kingSquare, targetSquare, Chess::KING, Chess::REMOVE_ALL_CASTLING, Constants::SQUARE_BBS[targetSquare] & enemyPieces));
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
                else moves->addMove(Move(kingSquare, targetCastlingSquare, Chess::KING, Chess::SHORT_CASTLING));
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
                else moves->addMove(Move(kingSquare, targetCastlingSquare, Chess::KING, Chess::LONG_CASTLING));
        }

        return moveCount;
    }
    template <bool whiteToMove, bool countOnly>
    uint8_t genKnightMoves(const Game& game, Moves<>* moves) {
        Index startSquare, targetSquare;
        bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard movesBitboard = 0ULL, enemyOrEmpty = game.board.notFriendlyPieces<whiteToMove>();
		bitboard knights = game.board.getKnights<whiteToMove>();
        uint8_t moveCount = 0;

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
                    moves->addMove(Move(startSquare, targetSquare, Chess::KNIGHT, Chess::QUIET, Constants::SQUARE_BBS[targetSquare] & enemyPieces));
                }
            }
        }

        return moveCount;
    }

    template <bool whiteToMove, bool countOnly>
    uint8_t genBishopMoves(const Game& game, Moves<>* moves) {
        Index startSquare, targetSquare;
        bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard movesBitboard = 0ULL, enemyOrEmpty = game.board.notFriendlyPieces<whiteToMove>();
        bitboard allPieces = game.board.getAllPieces(), bishops = game.board.getBishops<whiteToMove>();
        uint8_t moveCount = 0;

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
                    moves->addMove(Move(startSquare, targetSquare, Chess::BISHOP, Chess::QUIET, Constants::SQUARE_BBS[targetSquare] & enemyPieces));
                }
            }
        }

        return moveCount;
    }

    template <bool whiteToMove, bool countOnly>
    uint8_t genQueenMoves(const Game& game, Moves<>* moves) {
        Index startSquare, targetSquare;
        bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard movesBitboard = 0ULL, enemyOrEmpty = game.board.notFriendlyPieces<whiteToMove>();
		bitboard allPieces = game.board.getAllPieces(),  queens = game.board.getQueens<whiteToMove>();
        uint8_t moveCount = 0;

        while (queens != 0) {
            startSquare = Chess::popLSB(queens);
            movesBitboard = PseudoMoveGen::getPseudoQueenMoves(enemyOrEmpty, startSquare, allPieces);
            movesBitboard = MoveGen::movesLegalityWhileChecked<whiteToMove>(game, startSquare, movesBitboard);
            movesBitboard = MoveGen::reducePinnedPiecesMoves<whiteToMove>(game, startSquare, movesBitboard);

            if constexpr (countOnly) moveCount += Chess::numOfBits(movesBitboard);
            else {
                while (movesBitboard != 0ULL) {
                    targetSquare = Chess::popLSB(movesBitboard);
                    moves->addMove(Move(startSquare, targetSquare, Chess::QUEEN, Chess::QUIET, Constants::SQUARE_BBS[targetSquare] & enemyPieces));
                }
            }
        }
        return moveCount;
    }

    template <bool whiteToMove, bool countOnly>
    uint8_t genRookMoves(const Game& game, Moves<>* moves) {
        Flag flag;
        Index startSquare, targetSquare;
        bitboard enemyPieces = game.board.getEnemyPieces<whiteToMove>();
        bitboard startSquareBB, movesBitboard = 0ULL, enemyOrEmpty = game.board.notFriendlyPieces<whiteToMove>();
        bitboard allPieces = game.board.getAllPieces(), rooks = game.board.getRooks<whiteToMove>();
        uint8_t moveCount = 0;

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

                    moves->addMove(Move(startSquare, targetSquare, Chess::ROOK, flag, Constants::SQUARE_BBS[targetSquare] & enemyPieces));
                }
            }
        }

        return moveCount;
    }
}
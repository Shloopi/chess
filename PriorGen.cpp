#include "PriorGen.hpp"
#include <iostream>
#include "Constants.hpp"

void PriorGen::Init() {

    std::array<Square, 4> diagDirs = { Square(1, 1), Square(1, -1), Square(-1, 1), Square(-1, -1) };
    std::array<Square, 4> orthoDirs = { Square(1, 0), Square(-1, 0), Square(0, 1), Square(0, -1) };

    PriorGen::genBetweenTable();
    PriorGen::genBetweenPiecesTable();
    PriorGen::genKnightMoves();
    PriorGen::genKingMoves();
    PriorGen::genDiagBasicMoves(diagDirs);
    PriorGen::genOrthoBasicMoves();
    PriorGen::genLookupTable(true, diagDirs);
    PriorGen::genLookupTable(false, orthoDirs);
}

void PriorGen::genKnightMoves() {
    std::array<Square, 8> directions = { Square(1, 2), Square(1, -2), Square(-1, 2), Square(-1, -2), Square(2, 1), Square(2, -1), Square(-2, 1), Square(-2, -1) };

    bitboard moves;
    Square sourceSquare, targetSquare;

    for (int i = 0; i < PriorGen::knightMoves.size(); i++) {
        moves = 0;
        sourceSquare = Square(i);

        for (int j = 0; j < directions.size(); j++) {
            targetSquare = Square::addSquares(sourceSquare, directions[j]);

            if (targetSquare.isValid()) {
                moves |= SQUARE_BBS[targetSquare.index];
            }
        }

        PriorGen::knightMoves[i] = moves;
    }
}

void PriorGen::genKingMoves() {
    std::array<Square, 8> directions = { Square(1, 1), Square(1, -1), Square(-1, 1), Square(-1, -1), Square(1, 0), Square(-1, 0), Square(0, 1), Square(0, -1) };
    bitboard moves;
    Square sourceSquare, targetSquare;

    for (int i = 0; i < PriorGen::kingMoves.size(); i++) {
        moves = 0;
        sourceSquare = Square(i);

        for (int j = 0; j < directions.size(); j++) {
            targetSquare = Square::addSquares(sourceSquare, directions[j]);

            if (targetSquare.isValid()) {
                moves |= SQUARE_BBS[targetSquare.index];
            }
        }

        PriorGen::kingMoves[i] = moves;
    }
}
void PriorGen::genLookupTable(bool isDiag, const std::array<Square, 4>& directions) {
    std::array<bitboard, Chess::BOARD_SIZE>* basicMovesPtr = nullptr;
    std::array<std::vector<bitboard>, Chess::BOARD_SIZE>* lookupTablePtr = nullptr;

    if (isDiag) {
        basicMovesPtr = &PriorGen::diagBasicMoves;
        lookupTablePtr = &PriorGen::diagLookupTable;
    }
    else {
        basicMovesPtr = &PriorGen::orthoBasicMovesForMagic;
        lookupTablePtr = &PriorGen::orthoLookupTable;
    }

    std::array<bitboard, Chess::BOARD_SIZE>& basicMoves = *basicMovesPtr;
    std::array<std::vector<bitboard>, Chess::BOARD_SIZE>& lookupTable = *lookupTablePtr;

    bitboard moves, attack, magicNumber;
    std::vector<bitboard> occupancies;
    Square sourceSquare;
    int magicShifter;
    Index lookupTableIndex;

    for (int i = 0; i < basicMoves.size(); i++) {
        sourceSquare = Square(i);
        moves = basicMoves[i];
        if (isDiag) {
            magicShifter = BISHOP_SHIFTERS[i];
            magicNumber = BISHOP_MAGIC_NUMBERS[i];
            moves &= ~Bitboard::RANK0 & ~Bitboard::RANK7 & ~Bitboard::FILE_A & ~Bitboard::FILE_H;
        }
        else {
            magicShifter = ROOK_SHIFTERS[i];
            magicNumber = ROOK_MAGIC_NUMBERS[i];
        }

        lookupTable[i].resize(SQUARE_BBS[64 - magicShifter]);

        occupancies = PriorGen::genOccupancies(moves);

        for (int j = 0; j < occupancies.size(); j++) {
            lookupTableIndex = PriorGen::genMagicIndex(occupancies[j], magicNumber, magicShifter);
            attack = PriorGen::genSlidingAttack(sourceSquare, occupancies[j], directions);

            lookupTable[i][lookupTableIndex] = attack;
        }
    }
}
void PriorGen::genBetweenTable() {
    bitboard ray;
    short fromRank, fromFile, toRank, toFile, tempRank, tempFile;

    for (short from = 0; from < Chess::BOARD_SIZE; from++) {
        for (short to = 0; to < Chess::BOARD_SIZE; to++) {
            // No squares between a square and itself
            if (from == to) {
                PriorGen::betweenTable[from][to] = 0ULL;
                continue;
            }

            fromRank = from / Chess::RANK_SIZE;
            fromFile = from % Chess::RANK_SIZE;
            toRank = to / Chess::RANK_SIZE;
            toFile = to % Chess::RANK_SIZE;
            ray = 0ULL;

            int dr = (toRank - fromRank) == 0 ? 0 : (toRank - fromRank) / abs(toRank - fromRank);
            int df = (toFile - fromFile) == 0 ? 0 : (toFile - fromFile) / abs(toFile - fromFile);

            // Only proceed if aligned (same rank, file, or diagonal)
            if ((dr == 0 || df == 0 || (abs(dr) == abs(df)) && !(dr == 0 && df == 0))) {
                tempRank = fromRank + dr;
                tempFile = fromFile + df;

                // Go backwards to edge 
                while (tempRank >= 0 && tempRank < Chess::RANK_SIZE && tempFile >= 0 && tempFile < Chess::RANK_SIZE) {
                    ray |= SQUARE_BBS[tempRank * Chess::RANK_SIZE + tempFile];

                    tempRank -= dr;
                    tempFile -= df;
                }
                // Reset to original and go forwards to edge 
                tempRank = fromRank + dr;
                tempFile = fromFile + df;

                while (tempRank >= 0 && tempRank < Chess::RANK_SIZE && tempFile >= 0 && tempFile < Chess::RANK_SIZE) {
                    ray |= SQUARE_BBS[tempRank * Chess::RANK_SIZE + tempFile];
                    tempRank += dr;
                    tempFile += df;
                }
            }

            PriorGen::betweenTable[from][to] = ray;
        }
    }
}

void PriorGen::genBetweenPiecesTable() {
    bitboard ray;
    short tempRank, tempFile, fromRank, fromFile, toRank, toFile, minRank, minFile, maxRank, maxFile;

    for (short from = 0; from < Chess::BOARD_SIZE; from++) {
        for (short to = 0; to < Chess::BOARD_SIZE; to++) {
            // No squares between a square and itself
            if (from == to) {
                PriorGen::betweenPiecesTable[from][to] = 0ULL;
                continue;
            }

            fromRank = from / Chess::RANK_SIZE;
            fromFile = from % Chess::RANK_SIZE;

            toRank = to / Chess::RANK_SIZE;
            toFile = to % Chess::RANK_SIZE;

            minRank = std::min(fromRank, toRank);
            minFile = std::min(fromFile, toFile);
            maxRank = std::max(fromRank, toRank);
            maxFile = std::max(fromFile, toFile);

            ray = 0ULL;

            int rankDerivative = (toRank - fromRank) == 0 ? 0 : (toRank - fromRank) / abs(toRank - fromRank);
            int fileDerivative = (toFile - fromFile) == 0 ? 0 : (toFile - fromFile) / abs(toFile - fromFile);


            // Only proceed if aligned (same rank, file, or diagonal).
            if (rankDerivative == 0 || fileDerivative == 0 || abs(static_cast<short>(to - from)) % 7 == 0 || abs(static_cast<int>(to - from)) % 9 == 0) {

                tempRank = fromRank + rankDerivative;
                tempFile = fromFile + fileDerivative;

                // Go backwards to edge 
                while ((tempRank < maxRank && tempRank > minRank) || (tempFile < maxFile && tempFile > minFile)) {
                    ray |= (1ULL << (tempRank * Chess::RANK_SIZE + tempFile));
                    tempRank += rankDerivative;
                    tempFile += fileDerivative;
                }
            }
            PriorGen::betweenPiecesTable[from][to] = ray;
        }
    }
}

bitboard PriorGen::genSlidingAttack(Square sourceSquare, bitboard occupancy, const std::array<Square, 4>& directions) {
    bitboard currMove, attacks = 0ULL;
    Square targetSquare;

    for (int k = 0; k < directions.size(); k++) {
        targetSquare = Square::addSquares(sourceSquare, directions[k]);

        while (targetSquare.isValid()) {
            currMove = SQUARE_BBS[targetSquare.index];
            attacks |= currMove;
            targetSquare = Square::addSquares(targetSquare, directions[k]);

            if ((currMove & occupancy) != 0) {
                break;
            }
        }
    }

    return attacks;
}
void PriorGen::genDiagBasicMoves(const std::array<Square, 4>& directions) {
    bitboard moves;
    Square sourceSquare, targetSquare;

    for (int i = 0; i < PriorGen::diagBasicMoves.size(); i++) {
        sourceSquare = Square(i);
        moves = 0ULL;

        for (int j = 0; j < directions.size(); j++) {
            targetSquare = Square::addSquares(sourceSquare, directions[j]);

            while (targetSquare.isValid()) {
                moves |= SQUARE_BBS[targetSquare.index];
                targetSquare = Square::addSquares(targetSquare, directions[j]);
            }
        }

        PriorGen::diagBasicMoves[i] = moves;
    }
}
void PriorGen::genOrthoBasicMoves() {
    Index rank, file, i;
    bitboard bbRank, bbFile;

    // loop for the number of squares.
    for (i = 0; i < PriorGen::orthoBasicMovesForMagic.size(); i++) {
        rank = i / Chess::RANK_SIZE;
        file = i % Chess::RANK_SIZE;

        // get the bitboard of the file and rank of the current square.
        // remove the moves at the sides of the board.
        bbFile = (Bitboard::FILE_A << file);
        bbRank = (Bitboard::RANK0 << (rank * Chess::RANK_SIZE));

        // combine the file and rank bitboards and remove the current square from the bitboard.
        PriorGen::orthoBasicMoves[i] = (bbFile | bbRank) & ~SQUARE_BBS[i];

        bbFile &= ~Bitboard::RANK0 & ~Bitboard::RANK7;
        bbRank &= ~Bitboard::FILE_A & ~Bitboard::FILE_H;
        PriorGen::orthoBasicMovesForMagic[i] = (bbFile | bbRank) & ~SQUARE_BBS[i];
    }
}
std::vector<bitboard> PriorGen::genOccupancies(bitboard moves) {
    std::vector<bitboard> occupancies(Constants::SQUARE_BBS[Bitboard::numOfBits(moves)]);
    bitboard currOccupancy = moves;

    for (int i = 0; i < occupancies.size(); i++) {
        occupancies[i] = currOccupancy;
        currOccupancy = (currOccupancy - 1) & moves;
    }

    return occupancies;
}
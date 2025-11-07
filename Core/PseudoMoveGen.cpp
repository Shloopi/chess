#include "PseudoMoveGen.hpp"

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
        int lookupTableIndex;

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
                if (lookupTableIndex > magicTable[i].size()) {
                    std::cout << lookupTableIndex;
                }

                magicTable[i][lookupTableIndex] = attack;
            }
        }
    }
}
#include <iostream>
#include "Zobrist.hpp"

namespace Zobrist {
    void init() {
        // generate numbers for each color, type of piece in each square.
        for (int i = 0; i < Zobrist::piecesRandom.size(); i++) {
            for (int j = 0; j < Zobrist::piecesRandom[i].size(); j++) {
                for (int k = 0; k < Zobrist::piecesRandom[i][j].size(); k++) {
                    Zobrist::piecesRandom[i][j][k] = Zobrist::genRandomNumber();
                }
            }
        }

        // generate numbers for each turn color.
        for (int i = 0; i < Zobrist::turnRandom.size(); i++) {
            Zobrist::turnRandom[i] = Zobrist::genRandomNumber();
        }

        // generate numbers for each castling rights combinations.
        for (int i = 0; i < Zobrist::castlingRandom.size(); i++) {
            Zobrist::castlingRandom[i] = Zobrist::genRandomNumber();
        }

        // generate numbers for each en passant file.
        for (int i = 0; i < Zobrist::enPassantFileRandom.size(); i++) {
            Zobrist::enPassantFileRandom[i] = Zobrist::genRandomNumber();
        }
    }

    uint64_t genRandomNumber()
    {
        Zobrist::seed += 0x9E3779B97F4A7C15ULL;

        uint64_t rand = Zobrist::seed;
        rand = (rand ^ (rand >> 30)) * 0xBF58476D1CE4E5B9ULL;
        rand = (rand ^ (rand >> 27)) * 0x94D049BB133111EBULL;

        return rand ^ (rand >> 31);
    }

    template uint64_t hash<true>(const Board& board);
    template uint64_t hash<false>(const Board& board);

    template <bool whiteToMove>
    uint64_t hash(const Board& board) {
        uint64_t hashKey = Zobrist::turnRandom[whiteToMove];

        hashKey ^= Zobrist::castlingRandom[board.castlingRights];
        hashKey ^= Zobrist::enPassantFileRandom[board.getEnPassantFile()];

        for (int i = 0; i < Chess::BOARD_SIZE; i++) {
            // White Pieces.
            if (Constants::SQUARE_BBS[i] & board.whitePawns) {
                hashKey ^= Zobrist::piecesRandom[true][Chess::PAWN][i];
            }
            else if (Constants::SQUARE_BBS[i] & board.whiteKnights) {
                hashKey ^= Zobrist::piecesRandom[true][Chess::KNIGHT][i];
            }
            else if (Constants::SQUARE_BBS[i] & board.whiteBishops) {
                hashKey ^= Zobrist::piecesRandom[true][Chess::BISHOP][i];
            }
            else if (Constants::SQUARE_BBS[i] & board.whiteRooks) {
                hashKey ^= Zobrist::piecesRandom[true][Chess::ROOK][i];
            }
            else if (Constants::SQUARE_BBS[i] & board.whiteQueens) {
                hashKey ^= Zobrist::piecesRandom[true][Chess::QUEEN][i];
            }
            else if (i == board.whiteKing) {
                hashKey ^= Zobrist::piecesRandom[true][Chess::KING][i];
            }
            // Black Pieces.
            else if (Constants::SQUARE_BBS[i] & board.blackPawns) {
                hashKey ^= Zobrist::piecesRandom[false][Chess::PAWN][i];
            }
            else if (Constants::SQUARE_BBS[i] & board.blackKnights) {
                hashKey ^= Zobrist::piecesRandom[false][Chess::KNIGHT][i];
            }
            else if (Constants::SQUARE_BBS[i] & board.blackBishops) {
                hashKey ^= Zobrist::piecesRandom[false][Chess::BISHOP][i];
            }
            else if (Constants::SQUARE_BBS[i] & board.blackRooks) {
                hashKey ^= Zobrist::piecesRandom[false][Chess::ROOK][i];
            }
            else if (Constants::SQUARE_BBS[i] & board.blackQueens) {
                hashKey ^= Zobrist::piecesRandom[false][Chess::QUEEN][i];
            }
            else if (i == board.blackKing) {
                hashKey ^= Zobrist::piecesRandom[false][Chess::KING][i];
            }
        }
        return hashKey;
    }
}


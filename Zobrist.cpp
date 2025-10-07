#include <iostream>
#include "Zobrist.hpp"

namespace Zobrist {
    //std::array<bitboard, 2> Zobrist::turnRandom{};
    //bitboard Zobrist::seed = 0ULL;
    //std::array<std::array<std::array<bitboard, 64>, 6>, 2> Zobrist::piecesRandom{};
    //std::array<bitboard, 16> Zobrist::castlingRandom{};
    //std::array<bitboard, 9> Zobrist::enPassantFileRandom{};

    void init() {
        // generate numbers for each color, type of piece in each square.
        for (int i = 0; i < Zobrist::piecesRandom.size(); i++) {
            for (int j = 0; j < Zobrist::piecesRandom[i].size(); j++) {
                Zobrist::piecesRandom[i][j] = Zobrist::genRandomNumber();
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
    bitboard Zobrist::genKey(bool whiteToMove) { return Zobrist::turnRandom[whiteToMove]; }

    bitboard Zobrist::applyPiece(bitboard key, Piece piece, Index square) {
        bitboard pieceRandom = Zobrist::piecesRandom[piece][square];

        key ^= pieceRandom;

        return key;
    }

    bitboard Zobrist::applyBoardInfo(bitboard key, const BoardInfo& info) {
        // apply castling rights.
        // Pack them into an index.
        key ^= Zobrist::castlingRandom[(info.whiteLeftCastle << 3) | (info.whiteRightCastle << 2) | (info.blackLeftCastle << 1) | (info.blackRightCastle << 0)];
        key ^= Zobrist::enPassantFileRandom[info.enPassantTarget < Chess::FILE_SIZE ? info.enPassantTarget / Chess::FILE_SIZE : Chess::FILE_SIZE];
        return key;
    }

    bitboard Zobrist::applyTurn(bitboard key, bool whiteToMove) {
        return key ^ Zobrist::turnRandom[whiteToMove];
    }

    bitboard Zobrist::genRandomNumber()
    {
        Zobrist::seed += 0x9E3779B97F4A7C15ULL;

        bitboard rand = Zobrist::seed;
        rand = (rand ^ (rand >> 30)) * 0xBF58476D1CE4E5B9ULL;
        rand = (rand ^ (rand >> 27)) * 0x94D049BB133111EBULL;

        return rand ^ (rand >> 31);
    }
}


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
    bitboard Zobrist::genKey(bool whiteToMove) { return Zobrist::turnRandom[whiteToMove]; }

    bitboard Zobrist::applyPiece(bitboard key, bool color, Piece piece, Index square) {
        bitboard pieceRandom = Zobrist::piecesRandom[color][piece][square];

        key ^= pieceRandom;

        return key;
    }
    bitboard Zobrist::applyPiece(bitboard key, ColoredPiece piece, Index square) {
		return Zobrist::applyPiece(key, piece < 6, static_cast<Piece>(piece % 6), square);
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


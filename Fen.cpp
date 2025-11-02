#include "Fen.hpp"
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <sstream>

static void split(std::vector<std::string>& splittedFen, const std::string& str, char delimiter) {
    std::stringstream strStream(str);
    std::string substr;

    while (std::getline(strStream, substr, delimiter)) {
        splittedFen.push_back(substr);
    }
}
static Piece getPieceHelper(char c) {
    if (c == 'P' || c == 'p') return Chess::PAWN;
    if (c == 'N' || c == 'n') return Chess::KNIGHT;
    if (c == 'B' || c == 'b') return Chess::BISHOP;
    if (c == 'R' || c == 'r') return Chess::ROOK;
    if (c == 'Q' || c == 'q') return Chess::QUEEN;
    if (c == 'K' || c == 'k') return Chess::KING;
    throw std::invalid_argument("FenHandler::getPiece - invalid piece character");
}
static char getCharFromPiece(Piece piece, bool isWhite) {
    if (isWhite) {
        if (piece == Chess::PAWN) return 'P';
        if (piece == Chess::KNIGHT) return 'N';
        if (piece == Chess::BISHOP) return 'B';
        if (piece == Chess::ROOK) return 'R';
        if (piece == Chess::QUEEN) return 'Q';
        if (piece == Chess::KING) return 'K';
    }
    else {
        if (piece == Chess::PAWN) return 'p';
        if (piece == Chess::KNIGHT) return 'n';
        if (piece == Chess::BISHOP) return 'b';
        if (piece == Chess::ROOK) return 'r';
        if (piece == Chess::QUEEN) return 'q';
        if (piece == Chess::KING) return 'k';
    }
    
    throw std::invalid_argument("FenHandler::getPiece - invalid piece character" + (piece + '0'));
}

namespace Fen {
    bool handleFen(const std::string& fen, BoardState& state) {
        bool whiteToMove;

        std::vector<std::string> splittedFen;
        split(splittedFen, fen, ' ');

        if (splittedFen.size() != 6) throw std::invalid_argument("Fen::Fen - fen does not contains all variables - " + fen);

        generatePieces(state.board, splittedFen[0]);
        whiteToMove = splittedFen[1] == "w";
        generateCastlingRights(state.board, splittedFen[2]);
        state.board.enPassant = splittedFen[3] == "-" ? 0 : Constants::SQUARE_BBS[Square::getIndex(splittedFen[3])];
        state.halfmoves = std::stoi(splittedFen[4]);
        state.fullmoves = std::stoi(splittedFen[5]);

        return whiteToMove;
    }

    void generatePieces(Board& board, const std::string& fenPieces) {
        std::vector<std::string> rows;
        split(rows, fenPieces, '/');

        if (rows.size() != Chess::RANK_SIZE) throw std::invalid_argument("Fen::Fen - fen did not include " + std::to_string(Chess::RANK_SIZE) + " rows - " + fenPieces);

        // counter and index for the board.
        int counter = 0;

        for (int i = (rows.size() - 1); i >= 0; i--) {
            for (unsigned char c : rows[i]) {
                // if the character is a digit, add the squares to the counter.
                if (std::isdigit(c)) {
                    counter += (c - '0');
                }
                // otherwise, it's representing a piece type and color.
                else {
					Piece type = getPieceHelper(c);
                    
                    if (std::isupper(c)) {
                        board.setPiece<true>(type, counter);
                    }
                    else {
                        board.setPiece<false>(type, counter);
                    }

                    counter++;
                }
            }
        }
    }

    void generateCastlingRights(Board& board, const std::string_view& fenCastlingRights) {
        bool whiteLeftCastle = false;
        bool whiteRightCastle = false;
        bool blackLeftCastle = false;
        bool blackRightCastle = false;

        if (fenCastlingRights != "-") {
            for (char c : fenCastlingRights) {
                switch (c) {
                case 'K':
                    whiteRightCastle = true;
                    break;
                case 'Q':
                    whiteLeftCastle = true;
                    break;
                case 'k':
                    blackRightCastle = true;
                    break;
                case 'q':
                    blackLeftCastle = true;
                    break;
                default:
                    throw std::invalid_argument("FenHandler::generateCastlingRights - fen castling rights invalid character");
                }
            }
        }

		board.setCastlingRights(whiteLeftCastle, whiteRightCastle, blackLeftCastle, blackRightCastle);
    }

    template std::string genFen<true>(const BoardState& state);
    template std::string genFen<false>(const BoardState& state);

    template <bool whiteToMove>
    std::string genFen(const BoardState& state) {
        std::string fen;
        short count = 0;
        unsigned char c;
        Index square;
        bitboard squareBB;
        Piece piece;
        bitboard whitePieces = state.board.getAllPieces<true>();
        for (Index rank = Chess::RANK_SIZE - 1; rank >= 0; rank--) {
            for (Index file = 0; file < Chess::RANK_SIZE; file++) {
                square = rank * Chess::RANK_SIZE + file;
                squareBB = Constants::SQUARE_BBS[square];
                if (file == 0 && rank != Chess::RANK_SIZE - 1) {
                    if (count > 0) {
                        fen += (char)(count + '0');
                        count = 0;
                    }
                    fen += '/';
                }

                piece = state.board.getPieceAt(square);

                if (piece == -1) {
                    count++;
                }
                else {
                    if (count > 0) {
                        fen += (char)(count + '0');
                        count = 0;
                    }

                    // uppercase for white, lowercase for black
                    c = getCharFromPiece(piece, (whitePieces & squareBB) != 0);

                    fen += c;
                }
            }
        }

        // flush the last count if the final rank ends with empties
        if (count > 0) {
            fen += (char)(count + '0');
        }

        fen += ' ';

        // turn.
        fen += whiteToMove ? 'w' : 'b';

        fen += ' ';

        // castling rights.
        if (!state.board.canCastleShort<true>() && !state.board.canCastleLong<true>() &&
            !state.board.canCastleShort<false>() && !state.board.canCastleLong<false>()) {
            fen += '-';
        }
        else {
            if (state.board.canCastleShort<true>()) fen += 'K';
            if (state.board.canCastleLong<true>()) fen += 'Q';
            if (state.board.canCastleShort<false>()) fen += 'k';
            if (state.board.canCastleLong<false>()) fen += 'q';
        }

        fen += ' ';

        // en passant.
        if (state.board.enPassant == 0) fen += '-';
        else fen += Square::getNotation(state.board.getEnPassantSquare());

        fen += ' ';

        // halfmoves.
        fen += std::to_string(state.halfmoves);

        fen += ' ';

        // fullmoves.
        fen += std::to_string(state.fullmoves);

        return fen;
    }
}
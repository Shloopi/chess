#include "Fen.hpp"
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <sstream>
#include "Piece.hpp"

static void split(std::vector<std::string>& splittedFen, const std::string& str, char delimiter) {
    std::stringstream strStream(str);
    std::string substr;

    while (std::getline(strStream, substr, delimiter)) {
        splittedFen.push_back(substr);
    }
}

namespace Fen {
    void handleFen(const std::string& fen, std::array<bitboard, 12>& pieces, BoardInfo& state) {
        std::vector<std::string> splittedFen;
        split(splittedFen, fen, ' ');

        if (splittedFen.size() != 6) throw std::invalid_argument("Fen::Fen - fen does not contains all variables - " + fen);

        generatePieces(splittedFen[0], pieces);
        state.whiteMove = splittedFen[1] == "w";
        generateCastlingRights(splittedFen[2], state);
        state.enPassantTarget = splittedFen[3] == "-" ? -1 : Square::getIndex(splittedFen[3]);
        state.halfmoves = std::stoi(splittedFen[4]);
        state.fullmoves = std::stoi(splittedFen[5]);
    }

    void generatePieces(const std::string& fenPieces, std::array<bitboard, 12>& pieces) {
        for (int i = 0; i < pieces.size(); i++) pieces[i] = 0ULL;

        std::vector<std::string> rows;
        split(rows, fenPieces, '/');

        if (rows.size() != Chess::RANK_SIZE) throw std::invalid_argument("Fen::Fen - fen did not include " + std::to_string(Chess::RANK_SIZE) + " rows - " + fenPieces);

        // counter and index for the board.
        int counter = 0;

        for (int i = (rows.size() - 1); i >= 0; i--) {
            for (char c : rows[i]) {
                // if the character is a digit, add the squares to the counter.
                if (std::isdigit(c)) {
                    counter += (c - '0');
                }
                // otherwise, it's representing a piece type and color.
                else {
					ColoredPiece type = PieceHelper::getPiece(c);

                    // add the piece to the correct PieceList.
                    pieces[type] |= Constants::SQUARE_BBS[counter];

                    counter++;
                }
            }
        }
    }

    void generateCastlingRights(const std::string_view& fenCastlingRights, BoardInfo& state) {
        state.whiteLeftCastle = false;
        state.whiteRightCastle = false;
        state.blackLeftCastle = false;
        state.blackRightCastle = false;

        if (fenCastlingRights != "-") {
            for (char c : fenCastlingRights) {
                switch (c) {
                case 'K':
                    state.whiteRightCastle = true;
                    break;
                case 'Q':
                    state.whiteLeftCastle = true;
                    break;
                case 'k':
                    state.blackRightCastle = true;
                    break;
                case 'q':
                    state.blackLeftCastle = true;
                    break;
                default:
                    throw std::invalid_argument("FenHandler::generateCastlingRights - fen castling rights invalid character");
                }
            }
        }
    }
}
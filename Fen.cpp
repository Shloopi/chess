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

namespace Fen {
    bool handleFen(const std::string& fen, BoardState& state) {
        bool whiteToMove;

        std::vector<std::string> splittedFen;
        split(splittedFen, fen, ' ');

        if (splittedFen.size() != 6) throw std::invalid_argument("Fen::Fen - fen does not contains all variables - " + fen);

        generatePieces(state.board, splittedFen[0]);
        whiteToMove = splittedFen[1] == "w";
        generateCastlingRights(state.board, splittedFen[2]);
        state.board.enPassant = splittedFen[3] == "-" ? -1 : Square::getIndex(splittedFen[3]);
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
}
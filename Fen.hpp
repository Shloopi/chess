#ifndef FEN_HPP
#define FEN_HPP

#include <string>
#include <string_view>
#include "BoardState.hpp"
#include "Board.hpp"

namespace Fen {

    // generate from fen (all 6 parts of the fen).
    bool handleFen(const std::string& fen, BoardState& state);
    void generatePieces(Board& board, const std::string& fenPieces);
    void generateCastlingRights(Board& board, const std::string_view& fenCastlingRights);

    // get fen from position.
    //std::string genFen(const Board& board) { return ""; }
}

#endif
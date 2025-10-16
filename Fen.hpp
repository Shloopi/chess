#ifndef FEN_HPP
#define FEN_HPP

#include <string>
#include <array>
#include "BoardInfo.hpp"
#include "Board.hpp"

namespace Fen {

    // generate from fen (all 6 parts of the fen).
    void generatePieces(Board& board, const std::string& fenPieces);
    void generateCastlingRights(Board& board, const std::string_view& fenCastlingRights);
    Board handleFen(const std::string& fen, BoardInfo& state);

    // get fen from position.
    //std::string genFen(const Board& board) { return ""; }
}

#endif
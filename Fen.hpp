#ifndef FEN_HPP
#define FEN_HPP

#include <string>
#include <array>
#include "Bitboard.hpp"
#include "BoardInfo.hpp"

namespace Fen {

    // generate from fen (all 6 parts of the fen).
    void generatePieces(const std::string& fenPieces, std::array<bitboard, 12>& pieces);
    void generateCastlingRights(const std::string_view& fenCastlingRights, BoardInfo& state);
    void handleFen(const std::string& fen, std::array<bitboard, 12>& pieces, BoardInfo& state);

    // get fen from position.
    //std::string genFen(const Board& board) { return ""; }
}

#endif
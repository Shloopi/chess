#ifndef FEN_HPP
#define FEN_HPP

#include <string>
#include <string_view>
#include "../Core/Game.hpp"
#include "../Core/Board.hpp"

namespace Fen {

    // generate from fen (all 6 parts of the fen).
    void handleFen(const std::string& fen, Game& game);
    void generatePieces(Board& board, const std::string& fenPieces);
    void generateCastlingRights(Board& board, const std::string_view& fenCastlingRights);

    // get fen from position.
    std::string genFen(const Game& game);
}

#endif
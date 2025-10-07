#ifndef CHESS_HPP
#define CHESS_HPP

#include <cstdint>

using File = int64_t; // a number between 0 and 7 representing a file on the board.
using Rank = int64_t; // a number between 0 and 7 representing a rank on the board.
using Index = int64_t; // a number between 0 and 63 representing a square on the board.

namespace Chess {
	constexpr uint64_t BOARD_SIZE = 64;
	constexpr uint64_t RANK_SIZE = 8;
	constexpr uint64_t FILE_SIZE = 8;
};

struct Square {
	constexpr static File NULL_FILE = -1;
	constexpr static Rank NULL_RANK = -1;
	constexpr static Index NULL_INDEX = -1;

	File file;
	Rank rank;
	Index index;

	constexpr Square() : file(NULL_FILE), rank(NULL_RANK), index(NULL_INDEX) {}
	constexpr Square(File file, Rank rank) : file(file), rank(rank), index(rank * Chess::RANK_SIZE + file) {}
	Square(Index index) : file(index % Chess::RANK_SIZE), rank(index / Chess::RANK_SIZE), index(index) {}

	inline bool isValid() const {
		return this->file >= 0 && this->file < Chess::FILE_SIZE && this->rank >= 0 && this->rank < Chess::RANK_SIZE;
	}

	inline void addSquares(Square square) {
		this->rank += square.rank;
		this->file += square.file;
		this->index = this->rank * Chess::RANK_SIZE + this->file;
	}

	static inline Index getIndex(std::string_view notation) {
		File file = notation[0] - 'a';
		Rank rank = notation[1] - '1';
		return rank * Chess::RANK_SIZE + file;
	}

	static inline std::string getNotation(Index index) {
		Square s = Square(index);

		if (!s.isValid()) return "-";

		return std::string(1, 'a' + s.file) + std::string(1, '1' + s.rank);
	}
};

#endif

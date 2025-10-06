#ifndef CHESS_HPP
#define CHESS_HPP

#include <cstdint>

using File = int64_t;
using Rank = int64_t;
using Index = int64_t;
namespace Chess {
	constexpr uint64_t BOARD_SIZE = 64;
	constexpr uint64_t RANK_SIZE = 8;
	constexpr uint64_t FILE_SIZE = 8;
}

struct Square {
	constexpr static File NULL_FILE = -1;
	constexpr static Rank NULL_RANK = -1;
	constexpr static Index NULL_INDEX = -1;

	File file;
	Rank rank;
	Index index;

	Square() : file(NULL_FILE), rank(NULL_RANK), index(NULL_INDEX) {}
	Square(File file, Rank rank) : file(file), rank(rank), index(rank * Chess::RANK_SIZE + file) {}
	Square(Index index) : file(index % Chess::RANK_SIZE), rank(index / Chess::RANK_SIZE), index(index) {}

	inline bool isValid() const {
		return this->file >= 0 && this->file < Chess::FILE_SIZE && this->rank >= 0 && this->rank < Chess::RANK_SIZE;
	}

	inline static Square addSquares(Square square1, Square square2) {
		Rank rank = square1.rank + square2.rank;
		File file = square1.file + square2.file;

		return rank < Chess::RANK_SIZE && file < Chess::RANK_SIZE ? Square(rank, file) : Square();
	}
};

#endif

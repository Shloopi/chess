#ifndef TEST_GEN_HPP
#define TEST_GEN_HPP

#include "Board.hpp"

namespace test {
	uint64_t timeDepth(Board& board, unsigned char depth = 5);
	uint64_t countMoves(Board& board, unsigned char depth = 5);

}


#endif
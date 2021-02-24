/**
 * All functions pertaining to operations on bit boards live in this file.
 * Bitboards are used for concice representation of the entire chessboard
 * within one unsigned 64 bit integer.
 * See https://en.wikipedia.org/wiki/Bitboard for more info.
 * 
 * Lots of this stuff comes from the chess programming wiki, in case
 * you're interested. Specifically, from here:
 * https://www.chessprogramming.org/Looking_for_Magics
 */

#include "constants.h"

// why this order? look it up
// I just kind of said "...okay" and used it
static const int32_t BIT_TABLE[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};

/**
 * Function that returns the index of the LSB in the bitboard that is set
 * to 1, while setting said index to 0
 * This is some more bit manipulation magic that I can't be bothered to
 * properly explain; hopefully the code is straightfoward enough?
 */
int32_t pop_bit(uint64_t *board_ptr) {
  uint64_t bit = *board_ptr ^ (*board_ptr - 1);

  uint32_t fold = (uint32_t)((bit & 0xFFFFFFFF) ^ (bit >> 32));

  *board_ptr &= (*board_ptr - 1);

  return BIT_TABLE[(fold * MAGIC_BIT_NUM) >> 26];
}

/**
 * Function for counting the number of set bits in a given bitboard
 * (but you probably already could tell that, from the name)
 */
int32_t count_bits(uint64_t board) {
  int32_t result;

  // weird looking, but this pretty much decrements one from the board
  // in a loop that does nothing else
  for(result = 0; board; result++, board &= board - 1);

  return result;
}

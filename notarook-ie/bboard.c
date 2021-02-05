/**
 * All functions pertaining to operations on bit boards live in this file.
 * Bitboards are used for concice representation of the entire chessboard
 * within one unsigned 64 bit integer.
 * See https://en.wikipedia.org/wiki/Bitboard for more info.
 * 
 * Lots of this stuff comes from the chess programming wiki, in case
 * you're interested
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

/**
 * Function to print out the internal representation of the passed in
 * bitboard for human-readable viewing
 */
void print_bboard(const uint64_t board) {

  int32_t rank, file, sq, sq64;
  rank = file = sq = sq64 = 0;

  printf("\n");

  // run through the board printing every value in
  // a way that looks like, you know, a chessboard
  for(rank = RANK_8; rank >= RANK_1; --rank) {
    for(file = FILE_A; file <= FILE_H; ++file) {
      sq = CONVERT_COORDS(file, rank); // engine coords
      sq64 = ENGINE_TO_REGULAR[sq]; // convert to standard coords

      // if we shift to the corresponding index and bitwise AND
      // it and get a 1, there is a piece there.
      if(CHECK_TAGGED(sq64, board)) printf("X");
      else printf("-");
    }
    printf("\n");
  }
  printf("\n\n");
}

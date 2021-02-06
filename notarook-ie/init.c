/**
 * This file just contains some definitions of functions
 * and variables declared in the header files that assist with
 * board setup and initialization.
 */

#include "constants.h"

// these are initially declared in constants.h
int32_t ENGINE_TO_REGULAR[BOARD_SQ_NUM];
int32_t REGULAR_TO_ENGINE[STANDARD_BOARD_SIZE];

uint64_t SET_MASK[STANDARD_BOARD_SIZE];
uint64_t CLEAR_MASK[STANDARD_BOARD_SIZE];

uint64_t PIECE_KEYS[13][BOARD_SQ_NUM];
uint64_t SIDE_KEY;
uint64_t CASTLE_KEYS[16];

/**
 * Function to generate a random 64 bit unsigned integer
 */
uint64_t RAND_64(void) {
  uint64_t ret = rand();
  ret += ((uint64_t)rand() << 15);
  ret += ((uint64_t)rand() << 30);
  ret += ((uint64_t)rand() << 45);
  ret += (((uint64_t)rand() & 0xF) << 60);
  return ret;
}

/**
 * Function to intialize the random keys
 */
static void init_hashkeys(void) {
  for(int32_t ind = 0; ind < 13; ++ind) {
    for(int32_t dex = 0; dex < 120; ++dex) {
      PIECE_KEYS[ind][dex] = RAND_64();
    }
  }

  SIDE_KEY = RAND_64();

  for(int32_t ind = 0; ind < 16; ++ind) {
    CASTLE_KEYS[ind] = RAND_64();
  }
}

/**
 * Function to initialize the bit mask arrays
 */
static void init_bit_masks(void) {

  // zero everything out, to start
  memset(SET_MASK, (uint64_t)0, STANDARD_BOARD_SIZE);
  memset(CLEAR_MASK, (uint64_t)0, STANDARD_BOARD_SIZE);

  // flip flop the clearmask and setmask so that you can actually
  // clear and set stuff
  for(int32_t index = 0; index < STANDARD_BOARD_SIZE; ++index) {
    SET_MASK[index] |= ((uint64_t)1 << index);
    CLEAR_MASK[index] = ~SET_MASK[index];
  }
}

/**
 * Function to initialize our engine and piece arrays
 * Clears out the arrays by setting them first to impossible values, then
 * continues on by initializing the actual values
 */
static void init_120_to_64(void) {

  int32_t square = A1;
  int32_t sq64 = 0;

  // initialize our placement arrays to impossible values, just to start out
  for(int32_t index = 0; index < BOARD_SQ_NUM; ++index)
    ENGINE_TO_REGULAR[index] = 65;

  memset(REGULAR_TO_ENGINE, 120, STANDARD_BOARD_SIZE);

  // for each square in the actual board
  // fill in the number coordinates for mapping back and forth
  for(int32_t rank = RANK_1; rank <= RANK_8; ++rank) {
    for(int32_t file = FILE_A; file <= FILE_H; ++file) {
      square = CONVERT_COORDS(file, rank);
      REGULAR_TO_ENGINE[sq64] = square;
      ENGINE_TO_REGULAR[square] = sq64;
      ++sq64;
    }
  }
}

/**
 * Initialize everything.
 * Externally visible function used in the main engine.
 */
void init_all(void) {
  init_120_to_64();
  init_bit_masks();
  init_hashkeys();
}

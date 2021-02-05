/**
 * This file just contains some definitions of functions
 * and variables declared in the header files that assist with
 * board setup and initialization.
 */

#include "constants.h"

// these are initially declared in constants.h
int32_t ENGINE_TO_REGULAR[BOARD_SQ_NUM];
int32_t REGULAR_TO_ENGINE[STANDARD_BOARD_SIZE];

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
}

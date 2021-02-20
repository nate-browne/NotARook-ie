/**
 * This file just contains some definitions of functions
 * and variables declared in the header files that assist with
 * board setup and initialization.
 */

#include "functions.h"
#include "constants.h"

// these are initially declared in constants.h
int32_t ENGINE_TO_REGULAR[BOARD_SQ_NUM];
int32_t REGULAR_TO_ENGINE[STANDARD_BOARD_SIZE];

uint64_t SET_MASK[STANDARD_BOARD_SIZE];
uint64_t CLEAR_MASK[STANDARD_BOARD_SIZE];

uint64_t PIECE_KEYS[13][BOARD_SQ_NUM];
uint64_t SIDE_KEY;
uint64_t CASTLE_KEYS[16];

int32_t FILES_BOARD[BOARD_SQ_NUM];
int32_t RANKS_BOARD[BOARD_SQ_NUM];

uint64_t FILE_BB_MASK[8];
uint64_t RANK_BB_MASK[8];

uint64_t BLACK_PASSED_MASK[STANDARD_BOARD_SIZE];
uint64_t WHITE_PASSED_MASK[STANDARD_BOARD_SIZE];
uint64_t ISOLATED_MASK[STANDARD_BOARD_SIZE];

/**
 * Initializes the bitboard masks for pawn evaluation
 */
static void init_eval_masks(void) {
  int32_t sq, tsq, rank, file;

  // zero out the passed pawn/isolated pawn masks
  for(sq = 0; sq < STANDARD_BOARD_SIZE; ++sq) {
    ISOLATED_MASK[sq] = 0;
    WHITE_PASSED_MASK[sq] = 0;
    BLACK_PASSED_MASK[sq] = 0;
  }

  // initialize them
  // you can use the print_bboard function to see what these end up
  // being
  for(sq = 0; sq < STANDARD_BOARD_SIZE; ++sq) {
    tsq = sq + 8;
    while(tsq < STANDARD_BOARD_SIZE) {
      WHITE_PASSED_MASK[sq] |= (1 << tsq);
      tsq += 8;
    }

    tsq = sq - 8;
    while(tsq >= 0) {
      BLACK_PASSED_MASK[sq] |= (1 << tsq);
      tsq -= 8;
    }

    if(FILES_BOARD[SQ120(sq)] > FILE_A) {
      ISOLATED_MASK[sq] |= FILE_BB_MASK[FILES_BOARD[SQ120(sq)] - 1];

      tsq = sq + 7;
      while(tsq < 64) {
        WHITE_PASSED_MASK[sq] |= (1 << tsq);
        tsq += 8;
      }

      tsq = sq - 9;
      while(tsq >= 0) {
        BLACK_PASSED_MASK[sq] |= (1 << tsq);
        tsq -= 8;
      }
    }

    if(FILES_BOARD[SQ120(sq)] < FILE_H) {
      ISOLATED_MASK[sq] |= FILE_BB_MASK[FILES_BOARD[SQ120(sq)] + 1];

      tsq = sq + 9;
      while(tsq < 64) {
        WHITE_PASSED_MASK[sq] |= (1 << tsq);
        tsq += 8;
      }

      tsq = sq - 7;
      while(tsq >= 0) {
        BLACK_PASSED_MASK[sq] |= (1 << tsq);
        tsq -= 8;
      }
    }
  }

  // clear out everything
  for(sq = 0; sq < 8; ++sq) {
    FILE_BB_MASK[sq] = 0;
    RANK_BB_MASK[sq] = 0;
  }

  // set the bits accordingly for the file and bit masks
  // going square by square
  for(rank = RANK_8; rank >= RANK_1; --rank) {
    for(file = FILE_A; file <= FILE_H; ++file) {
      sq = rank * 8 + file;
      FILE_BB_MASK[file] |= (1 << sq);
      RANK_BB_MASK[rank] |= (1 << sq);
    }
  }
}


/**
 * Function to generate a random 64 bit unsigned integer
 */
static uint64_t RAND_64(void) {
  uint64_t ret = rand();
  ret |= ((uint64_t)rand() << 15);
  ret |= ((uint64_t)rand() << 30);
  ret |= ((uint64_t)rand() << 45);
  ret |= (((uint64_t)rand() & 0xF) << 60);
  return ret;
}

/**
 * Function to initialize the files and ranks arrays
 */
static void init_files_ranks_arrays(void) {
  int32_t index, file, rank, sq;
  index = 0, file = FILE_A, rank = RANK_1, sq = A1;

  // first, set everything to offboard
  // this is so that we just fill in the relevant squares next
  for( ; index < BOARD_SQ_NUM; ++index)
    FILES_BOARD[index] = RANKS_BOARD[index] = OFFBOARD;

  // rank by rank, fill in the coordinates
  for(rank = RANK_1; rank <= RANK_8; ++rank) {
    for(file = FILE_A; file <= FILE_H; ++file) {
      sq = CONVERT_COORDS(file, rank);
      FILES_BOARD[sq] = file;
      RANKS_BOARD[sq] = rank;
    }
  }
}

/**
 * Function to intialize the random keys
 */
static void init_hashkeys(void) {
  for(int32_t ind = 0; ind < 13; ++ind) {
    for(int32_t dex = 0; dex < BOARD_SQ_NUM; ++dex) {
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
    ENGINE_TO_REGULAR[index] = STANDARD_BOARD_SIZE + 1;

  memset(REGULAR_TO_ENGINE, BOARD_SQ_NUM, STANDARD_BOARD_SIZE);

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
  init_files_ranks_arrays();
  init_eval_masks();
  init_MVV_LVA();
}

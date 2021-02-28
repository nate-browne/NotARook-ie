/**
 * A whole host of functions, the purpose of which are to validate that
 * stuff makes sense throughout the rest of the engine.
 *
 * Most of these validations have to do with the values seen in enums.h
 */

#include "constants.h"
#include "functions.h"

bool square_on_board(const int32_t square) {
  return FILES_BOARD[square] != OFFBOARD;
}

bool side_valid(const int32_t side) {
  return side == WHITE || side == BLACK;
}

// this function works for both files or ranks, since the enum values
// for valid board squares are between [0, 7]
bool file_rank_valid(const int32_t fr) {
  return fr >= 0 && fr <= 7;
}

bool piece_valid_empty(const int32_t piece) {
  return piece >= EMPTY && piece <= bK;
}

// difference between this and the above function is
// that piece_valid(empty) --> false, not true
bool piece_valid(const int32_t piece) {
  return piece >= wP && piece <= bK;
}

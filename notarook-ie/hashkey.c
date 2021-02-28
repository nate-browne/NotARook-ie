/**
 * File for interacting with the hashkey field of the
 * board and undo structs.
 *
 * This seems like it'd be a super complex maneuver, but
 * in reality all we're really doing is XORing the locations
 * of the pieces and options in the board.
 */

#include "constants.h"
#include "functions.h"

/**
 * Function that, given a board setup, generates the
 * hashkey for it
 */
uint64_t generate_hashkeys(const Board_t *board) {

  int32_t sq = 0, piece = EMPTY;
  uint64_t result = 0;

  // first, sum up the values of the pieces
  for( ; sq < BOARD_SQ_NUM; ++sq) {
    piece = board->pieces[sq];

    if(piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
      ASSERT(piece >= wP && piece <= bK);
      result ^= PIECE_KEYS[piece][sq];
    }
  }

  // hash in the side if it is white to move
  if(board->side == WHITE) result ^= SIDE_KEY;

  if(board->passant != NO_SQ) {
    ASSERT(board->passant >= 0 && board->passant < BOARD_SQ_NUM);

    // using empty for the 1st part of the array since we've used other parts
    result ^= PIECE_KEYS[EMPTY][board->passant];
  }

  // verify and hash in the castling permissions
  ASSERT(board->castle_permission >= 0 && board->castle_permission < 16);
  result ^= CASTLE_KEYS[board->castle_permission];

  return result;
}

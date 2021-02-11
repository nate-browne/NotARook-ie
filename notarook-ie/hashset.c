/**
 * Defines a hashmap for use in finding principal variations.
 * This speeds up the search process _dramatically_.
 *
 * Now is when I wish I did this project in python or C++
 */

#include "functions.h"
#include "constants.h"

/**
 * Function to get a full line of moves for a given board position,
 * storing them in the board for future reference
 */
int32_t get_pv_line(const int32_t depth, Board_t *board) {

  ASSERT(depth < MAX_DEPTH);

  uint32_t move = find_move(board);
  int32_t count = 0; // number of moves we're putting into the array

  while(move && count < depth) {
    ASSERT(count < MAX_DEPTH);

    if(move_exists(board, move)) {
      make_move(board, move);
      board->pv_array[count++] = move;
    } else {
      break; // illegal move
    }
    move = find_move(board);
  }

  // need to take the moves back to actually play the move we found
  while(board->ply > 0) {
    take_move(board);
  }
  return count;
}

/**
 * Initializes a hashset via output parameter
 */
void init_hashset(PVTable_t *pvt) {

  // how many entries can we hold
  pvt->entries = HASHSET_SIZE / sizeof(PVEntry_t);

  // double check this, may run into memory issues
  if(pvt->table) {
    free(pvt->table);
  }

  pvt->table = calloc(pvt->entries, sizeof(PVEntry_t));
}

/**
 * Zeros out a given hashset
 */
void clear_hashset(PVTable_t *pvt) {
  for(unsigned long index = 0; index < pvt->entries; ++index) {
    pvt->table[index].hashkey = (uint64_t)0;
    pvt->table[index].move = NOMOVE;
  }
}

/**
 * Uses the unique hashkey to store a move into the hashset.
 * This "hash function" isn't perfect for two reasons:
 * 1. It's unlikely, but it's not impossible for two different board positions
 * to have the same hashkey (we'd need to do a formal proof to prove that this isn't true)
 * 2. Two different hashkeys % the size could map to the same spot in the table.
 * We cross those bridges when/if we get there, though.
 */
void store_move(Board_t *board, const uint32_t move) {
  unsigned long index = board->hashkey % (board->pvt.entries);

  ASSERT(index >= 0 && index < board->pvt.entries);

  board->pvt.table[index].move = move;
  board->pvt.table[index].hashkey = board->hashkey;
}

/**
 * Essentially the inverse of the above function
 * Returns the move calculated for the given board position
 */
uint32_t find_move(const Board_t *board) {
  unsigned long index = board->hashkey % (board->pvt.entries);

  ASSERT(index >= 0 && index < board->pvt.entries);

  uint64_t found_hashkey = board->pvt.table[index].hashkey;

  return (found_hashkey == board->hashkey) ? board->pvt.table[index].move : NOMOVE;
}

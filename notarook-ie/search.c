/**
 * Functions for searching through the generated move trees
 */

#include "functions.h"
#include "constants.h"

/**
 * Called every so often to check if the time is up or the GUI
 * has sent an interrupt
 */
static void check_up() {
}

/**
 * Detects if a given board position is a repeat
 */
static bool is_repetition(const Board_t *board) {

  // little cool logical trick
  // move_counter is reset every time a pawn moves, meaning that we can't have a repeat
  // so we start counting from that point
  for(int32_t index = board->hist_ply - board->move_counter; index < board->hist_ply - 1; ++index) {

    ASSERT(index >= 0 && index < MAX_GAME_MOVES);

    if(board->hashkey == board->history[index].hashkey) return true;
  }

  return false;
}

/**
 * Clear relevant info for searching to get ready for a new search
 */
static void clear_for_search(SearchInfo_t *info, Board_t *board) {}

/**
 * Needed to avoid the horizon effect in which we don't calculate exchanges that are bad
 * for us.
 * Check the chess programming wiki for more about this
 */
static int32_t quiescence(int32_t alpha, int32_t beta, Board_t *board, SearchInfo_t *info) {
  return 0;
}

/**
 * Performs alpha beta tree search and pruning to find the best move
 */
static int32_t alpha_beta_search(int32_t alpha, int32_t beta, int32_t depth, Board_t *board, SearchInfo_t *info, bool null_allowed) {
  return 0;
}

/**
 * Does the iterative deepening and initializes the search information
 */
void search_position(Board_t *board, SearchInfo_t *info) {
}


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
static void clear_for_search(SearchInfo_t *info, Board_t *board) {
  for(int32_t idx = 0; idx < 13; ++idx) {
    for(int32_t idy = 0; idy < BOARD_SQ_NUM; ++idy) {
      board->search_history[idx][idy] = 0;
    }
  }

  for(int32_t idx = 0; idx < 2; ++idx) {
    for(int32_t idy = 0; idy < MAX_DEPTH; ++idy) {
      board->search_killers[idx][idy] = 0;
    }
  }

  clear_hashset(&board->pvt);
  board->ply = 0;

  info->starttime = get_time_millis();
  info->stopped = false;
  info->nodes = 0;
  info->fail_high = info->fail_high_first = 0.0;
}

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

  check_board(board);

  // evaluate the position if depth == 0 and return that
  if(!depth) {
    info->nodes++;
    return eval_position(board);
  }

  info->nodes++;

  // if we have a draw by repetition or by the 50 move rule
  if(is_repetition(board) || board->move_counter >= 100) {
    return 0;
  }

  // we've reached the deepest we will search in our board
  if(board->ply > MAX_DEPTH - 1) {
    return eval_position(board);
  }

  MoveList_t list;
  generate_all_moves(board, &list);

  int32_t legal = 0; // to check for checkmate or stalemate
  int32_t idx = 0;
  int32_t old_alpha = alpha;
  uint32_t best_move = NOMOVE;
  int32_t score = -INFINITY;

  for(idx = 0; idx < list.count; ++idx) {
    if(!make_move(board, list.moves[idx].move)) continue;

    ++legal;

    // negamax now
    // bounds are flipped cause we're looking from the opposite perspective
    score = -alpha_beta_search(-beta, -alpha, depth - 1, board, info, true);
    take_move(board);

    if(score > alpha) {
      if(score >= beta) {
        if(legal == 1) info->fail_high_first++;

        info->fail_high++;
        return beta; // beta cutoff
      }

      alpha = score;
      best_move = list.moves[idx].move;
    }
  }

  if(!legal) {
    if(square_attacked(board->kings_sq[board->side], board->side ^ 1, board)) {
      return -MATE + board->ply; // the ply gives us how many positions till mate
    } else {
      return 0; // stalemate
    }
  }

  if(alpha != old_alpha) {
    store_move(board, best_move);
  }

  return alpha;
}

/**
 * Does the iterative deepening search
 * Due to the idea of principle variation, this is more efficient than
 * directly searching a deep depth.
 */
void search_position(Board_t *board, SearchInfo_t *info) {
  uint32_t best_move = NOMOVE;
  int32_t best_score = -INFINITY;
  int32_t curr_depth = 0;
  int32_t pv_moves = 0;

  clear_for_search(info, board);

  // iterative deepening begins
  for(curr_depth = 1; curr_depth <= info->depth; ++curr_depth) {
    // first, find the best move
    best_score = alpha_beta_search(-INFINITY, INFINITY, curr_depth, board, info, true);


    // get (and print out) the pv line for the depth
    pv_moves = get_pv_line(curr_depth, board);

    // next, get the best move
    best_move = board->pv_array[0];
    fprintf(stderr, "Depth: %d score: %d move: %s, nodes: %ld ",
      curr_depth, best_score, print_move(best_move), info->nodes);

    fprintf(stderr, "pv: ");
    for(int32_t idx = 0; idx < pv_moves; ++idx) {
      fprintf(stderr, " %s", print_move(board->pv_array[idx]));
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "Ordering:%.2f\n", (info->fail_high_first / info->fail_high));
  }
}


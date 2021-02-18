/**
 * Functions for searching through the generated move trees
 */

#include "functions.h"
#include "constants.h"

/**
 * Called every so often to check if the time is up or the GUI
 * has sent an interrupt
 */
static void check_up(SearchInfo_t *info) {
  // we've run out of time
  if(info->timeset && get_time_millis() > info->stoptime) {
    info->stopped = true;
  }

  read_input(info);
}

/**
 * Swaps out the move we're looking at to find the better score at that point
 */
static void pick_next_move(int32_t move_num, MoveList_t *list) {
  Move_t temp;
  int32_t best_score = 0;
  int32_t index;
  int32_t best_num = move_num;

  // find the best move
  for(index = move_num; index < list->count; ++index) {
    if(list->moves[index].score > best_score) {
      best_score = list->moves[index].score;
      best_num = index;
    }
  }

  // swap out the move at our index for the best move
  temp = list->moves[move_num];
  list->moves[move_num] = list->moves[best_num];
  list->moves[best_num] = temp;
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

  check_board(board);

  // check every 2048 nodes to see if we've run out of time
  if(!(info->nodes & 2047)) check_up(info);

  info->nodes++;

  // if we have a draw by repetition or by the 50 move rule
  if(is_repetition(board) || board->move_counter >= 100) return 0;

  // we've reached the deepest we will search in our board
  if(board->ply > MAX_DEPTH - 1) return eval_position(board);

  int32_t score = eval_position(board);

  // if by doing nothing we have a better position, just return
  if(score >= beta) return beta;

  if(score > alpha) alpha = score;

  // this next bit is basically just alpha-beta search again
  MoveList_t list;
  generate_all_captures(board, &list);
  int32_t idx;
  int32_t legal = 0;
  int32_t old_alpha = alpha;
  uint32_t best_move = NOMOVE;
  score = -INFINITY;

  for(idx = 0; idx < list.count; ++idx) {
    pick_next_move(idx, &list);
    if(!make_move(board, list.moves[idx].move)) continue;

    ++legal;
    score = -quiescence(-beta, -alpha, board, info);
    take_move(board);

    if(info->stopped) return 0;

    // we found a good move
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

  // this means our best move now is better than our previous, so
  // put it as part of the best line
  if(alpha != old_alpha) {
    store_move(board, best_move);
  }

  return alpha;
}

/**
 * Performs alpha beta tree search and pruning to find the best move
 * For more information on null move pruning (the reason null_allowed is here),
 * check out this link: https://web.archive.org/web/20071031095933/http://www.brucemo.com:80/compchess/programming/nullmove.htm
 */
static int32_t alpha_beta_search(int32_t alpha, int32_t beta, int32_t depth, Board_t *board, SearchInfo_t *info, bool null_allowed) {

  check_board(board);

  // use quiescence to evaluate depth 0 to avoid the horizon effect
  if(!depth) return quiescence(alpha, beta, board, info);

  // check every 2048 nodes to see if we've run out of time
  if(!(info->nodes & 2047)) check_up(info);

  info->nodes++;

  // if we have a draw by repetition or by the 50 move rule
  if((is_repetition(board) || board->move_counter >= 100) && board->ply) return 0;

  // we've reached the deepest we will search in our board
  if(board->ply > MAX_DEPTH - 1) return eval_position(board);
 
  // add in some checks (hah) for if we're being checked at all
  // increase the depth because maybe there's a way out
  bool in_check = square_attacked(board->kings_sq[board->side], board->side ^ 1, board);
  if(in_check) ++depth;

  int32_t score = -INFINITY;

  // explaining each condition in order, we don't do recursive null moves (hence the param)
  // if we're in check, we can't do nothing (obviously)
  // make sure we have made at least one move in the search
  // avoid zugzwang positions by checking how many big pieces we have (king is a big piece)
  // make sure the engine depth is deep enough to make these (lack of) moves worth it
  if(null_allowed && !in_check && board->ply && (board->big_pieces[board->side] >= 2) && depth >= 4) {
    make_null_move(board);
    score = -alpha_beta_search(-beta, -beta + 1, depth - 4, board, info, false);
    take_null_move(board);
    if(info->stopped) return 0;

    // if our opponent's best move is greater than or equal to beta, this means that
    // our position is too stronk for them
    if(score >= beta) return beta;
  }

  MoveList_t list;
  generate_all_moves(board, &list);

  int32_t legal = 0; // to check for checkmate or stalemate
  int32_t idx = 0;
  int32_t old_alpha = alpha;
  uint32_t best_move = NOMOVE;
  score = -INFINITY;
  uint32_t pv_move = find_move(board);

  // we're still in the main line, so search that main line move first
  if(pv_move != NOMOVE) {
    for(idx = 0; idx < list.count; ++idx) {
      if(list.moves[idx].move == pv_move) {
        list.moves[idx].score = 2000000;
        break;
      }
    }
  }

  for(idx = 0; idx < list.count; ++idx) {

    pick_next_move(idx, &list);
    if(!make_move(board, list.moves[idx].move)) continue;

    ++legal;

    // negamax now
    // bounds are flipped cause we're looking from the opposite perspective
    score = -alpha_beta_search(-beta, -alpha, depth - 1, board, info, true);
    take_move(board);

    if(info->stopped) return 0;

    if(score > alpha) {
      if(score >= beta) {
        if(legal == 1) info->fail_high_first++;

        info->fail_high++;

        // non capture moves that cause beta cutoffs are killers
        if(!(list.moves[idx].move & MFLAGCAP)) {
          board->search_killers[1][board->ply] = board->search_killers[0][board->ply];
          board->search_killers[0][board->ply] = list.moves[idx].move;
        }
        return beta; // beta cutoff
      }

      alpha = score;
      best_move = list.moves[idx].move;

      // non capture moves that beat alpha are history improvers
      if(!(list.moves[idx].move & MFLAGCAP)) {
        board->search_history[board->pieces[FROMSQ(best_move)]][TOSQ(best_move)] += depth;
      }
    }
  }

  if(!legal) {
    if(in_check) {
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

    if(info->stopped) break;


    // get (and print out) the pv line for the depth
    pv_moves = get_pv_line(curr_depth, board);

    // next, get the best move
    best_move = board->pv_array[0];

    // print based on the mode
    if(info->game_mode == UCIMODE) {
      printf("info score cp %d depth %d nodes %ld time %lu ",
        best_score, curr_depth, info->nodes, get_time_millis() - info->starttime);
    } else if(info->game_mode == XBOARDMODE && info->post_thinking) {
      printf("%d %d %lu %ld ", curr_depth, best_score, (get_time_millis() - info->starttime) / 10, info->nodes);
    } else if(info->post_thinking) {
      printf("score:%d depth:%d nodes:%ld time:%lu(ms) ", best_score, curr_depth, info->nodes, get_time_millis() - info->starttime);
    }

    if(info->game_mode == UCIMODE || info->post_thinking) {
      pv_moves = get_pv_line(curr_depth, board);
      printf("pv");
      for(int32_t idx = 0; idx < pv_moves; ++idx) {
        printf(" %s", print_move(board->pv_array[idx]));
      }
      printf("\n");
    }
  }

  // UCI protocol dictates that all we do is print our best move
  if(info->game_mode == UCIMODE) {
    printf("bestmove %s\n", print_move(best_move));

  // for xboard protocol, we need to print the move (for the GUI to play)
  // as well as play it ourselves
  } else if(info->game_mode == XBOARDMODE) {
    printf("move %s\n", print_move(best_move));
    make_move(board, best_move);

  // for console, we print the move very visibly and play it, then print
  // the board for the user to see
  } else {
    printf("\n\n***!! NotARook-ie makes move %s !!***\n\n", print_move(best_move));
    make_move(board, best_move);
    print_board(board);
  }
}

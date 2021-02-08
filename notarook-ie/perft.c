/**
 * File used for perft testing (a.k.a. movegen stress testing)
 * For more about perft, check this link: https://www.chessprogramming.org/Perft
 * Basic idea is that we start from a known position and after a sequence of moves
 * we count the leaf nodes. Ideally, we know what the answer should be for some starting
 * position so that we'll be able to see definitively if our move generator is working.
 */

#include "functions.h"
#include "constants.h"

/**
 * Run a perft test for a given depth. Recursive to traverse every
 * branch of the possible move tree. Updates count as an output param
 * for the number of leaf nodes in our move tree
 */
static void perft(int32_t depth, Board_t *board, uint64_t *count) {

  check_board(board);

  if(depth < 1) {
    (*count)++;
    return;
  }

  MoveList_t list;
  generate_all_moves(board, &list);

  for(int32_t move_num = 0; move_num < list.count; ++move_num) {
    if(!make_move(board, list.moves[move_num].move)) continue;

    perft(depth - 1, board, count);
    take_move(board);
  }

  return;
}

/**
 * This function is the wrapper function for the above perft testing func.
 * If being run from the stresstest program, don't run the print statements here
 */
uint64_t perft_test(int32_t depth, Board_t *board, bool stress) {

  // always good to sanity check
  check_board(board);

  if(!stress) {
    print_board(board);
    printf("\nStarting perft test to depth: %d\n", depth);
  }

  uint64_t total = 0;

  MoveList_t list;
  generate_all_moves(board, &list);

  for(int32_t move_num = 0; move_num < list.count; ++move_num) {

    int32_t move = list.moves[move_num].move;

    if(!make_move(board, move)) continue;

    uint64_t leaves = 0;
    perft(depth - 1, board, &leaves);
    total += leaves;
    take_move(board);

    if(!stress) printf("move %d (%s): %lld possible positions\n", move_num + 1, print_move(move), leaves);
  }

  if(!stress) printf("\nTest complete: %lld leaf nodes visited.\n", total);

  return total;
}

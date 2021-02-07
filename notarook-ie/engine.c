/**
 * This file is for the main operations of the chess engine.
 * It uses all of the other code written to actually make
 * the magic happen.
 */

#include "constants.h"
#include "functions.h"

#define PAWNMOVES "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"

/**
 * The function that starts it all.
 */
int main(int argc, char * argv[]) {
  init_all();

  Board_t board;

  parse_FEN(PAWNMOVES, &board);
  print_board(&board);

  MoveList_t list;

  generate_all_moves(&board, &list);

  print_movelist(&list);

  return EXIT_SUCCESS;
}

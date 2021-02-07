/**
 * This file is for the main operations of the chess engine.
 * It uses all of the other code written to actually make
 * the magic happen.
 */

#include "constants.h"
#include "functions.h"


/**
 * The function that starts it all.
 */
int main(int argc, char * argv[]) {
  init_all();

  Board_t board;
  MoveList_t list;

  parse_FEN(TRICKY_FEN, &board);
  generate_all_moves(&board, &list);

  print_movelist(&list);

  return EXIT_SUCCESS;
}

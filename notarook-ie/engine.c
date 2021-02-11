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
int main(void) {
  init_all();

  Board_t board;
  board.pvt.table = NULL;
  init_hashset(&board.pvt);
  SearchInfo_t info;
  char input[6];
  int32_t move = NOMOVE;

  parse_FEN(MIN1, &board);

  while(true) {
    print_board(&board);

    printf("Please enter a move: ");
    fgets(input, sizeof(input), stdin);

    if(*input == 'q') break;

    if(*input == 't') {
      take_move(&board);
    } else if(*input == 'p') {
      perft_test(4, &board, false);
    } else if(*input == 's') {
      info.depth = 6;
      search_position(&board, &info);
    } else {
      move = parse_move(input, &board);
      if(move) {
        store_move(&board, move);
        make_move(&board, move);
      }
    }

    move = NOMOVE;
    fflush(stdin);
  }

  free(board.pvt.table);
  return EXIT_SUCCESS;
}

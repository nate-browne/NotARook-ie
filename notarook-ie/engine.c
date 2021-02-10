/**
 * This file is for the main operations of the chess engine.
 * It uses all of the other code written to actually make
 * the magic happen.
 */

#include "constants.h"
#include "functions.h"

#define TEMPFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"

/**
 * The function that starts it all.
 */
int main(void) {
  init_all();

  Board_t board;
  char input[6];
  int32_t move = NOMOVE;

  parse_FEN(TEMPFEN, &board);

  while(true) {
    print_board(&board);

    printf("Please enter a move: ");
    fgets(input, sizeof(input), stdin);

    if(*input == 'q') break;

    if(*input == 't') {
      take_move(&board);
    } else {
      move = parse_move(input, &board);
      if(move) make_move(&board, move);
    }

    move = NOMOVE;
    fflush(stdin);
  }

  return EXIT_SUCCESS;
}

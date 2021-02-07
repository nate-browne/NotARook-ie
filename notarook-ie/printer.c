/**
 * This file is for printing the moves and squares in algebraic
 * notation
 */

#include "constants.h"
#include "functions.h"

/**
 * Create a string that can be printed out with printf.
 */
char * print_square(const int32_t sqr) {
  static char square_ret_str[3];

  int32_t file = FILES_BOARD[sqr];
  int32_t rank = RANKS_BOARD[sqr];

  snprintf(square_ret_str, sizeof(square_ret_str), "%c%c", ('a' + file), ('1' + rank));

  return square_ret_str;
}

/**
 * Returns a string for printing out a move
 */
char * print_move(const int32_t move) {

  static char ret_mv_str[6];

  int32_t ff = FILES_BOARD[FROMSQ(move)];
  int32_t rf = RANKS_BOARD[FROMSQ(move)];
  int32_t ft = FILES_BOARD[TOSQ(move)];
  int32_t rt = RANKS_BOARD[TOSQ(move)];

  int32_t promoted = PROMOTED(move);

  if(promoted) {
    char prom_char = 'q';

    if(IsKn(promoted)) {
      prom_char = 'n';
    } else if(IsRQ(promoted) && !IsBQ(promoted)) {
      prom_char = 'r';
    } else if(!IsRQ(promoted) && IsBQ(promoted)) {
      prom_char = 'b';
    }
    snprintf(ret_mv_str, sizeof(ret_mv_str), "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), prom_char);
  } else {
    snprintf(ret_mv_str, sizeof(ret_mv_str), "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
  }

  return ret_mv_str;
}

/**
 * Prints the contents of the movelist in a readable way
 */
void print_movelist(const MoveList_t * list) {
  printf("MoveList:\n");

  for(int32_t index = 0; index < list->count; ++index) {
    int32_t move = list->moves[index].move;
    int32_t score = list->moves[index].score;

    printf("Move: %d %s (score: %d)\n", index + 1, print_move(move), score);
  }
  printf("MoveList Total %d Moves\n\n", list->count);
}

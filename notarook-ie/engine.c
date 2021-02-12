/**
 * This file is for the main operations of the chess engine.
 * It uses all of the other code written to actually make
 * the magic happen.
 */

#include "constants.h"
#include "functions.h"

#define WELCOME_STR "Welcome to %s! Type 'console' for console mode, 'uci' for UCI engine mode, or 'xboard' for xboard engine mode. \
Type 'quit' to exit.\n"

/**
 * The function that starts it all.
 */
int main(void) {
  init_all();

  Board_t board;
  SearchInfo_t info;
  board.pvt.table = NULL;
  init_hashset(&board.pvt);

  printf(WELCOME_STR, NAME);

  char line[BUFSIZ];

  while(true) {
    memset(line, '\0', BUFSIZ);

    fflush(stdout);

    if(!fgets(line, BUFSIZ, stdin)) continue;

    if(*line == '\n') continue;

    if(!strncmp(line, "uci", strlen("uci"))) {
      UCI_loop(&board, &info);
      if(info.quit) break;
      continue;
    } else if(!strncmp(line, "xboard", strlen("xboard"))) {
      XBoard_loop(&board, &info);
      if(info.quit) break;
      continue;
    } else if(!strncmp(line, "console", strlen("console"))) {
      console_loop(&board, &info);
      if(info.quit) break;
      continue;
    } else if(!strncmp(line, "quit", strlen("quit"))) {
      break;
    }
  }


  free(board.pvt.table);
  board.pvt.table = NULL;

  return EXIT_SUCCESS;
}

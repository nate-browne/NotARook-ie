/**
 * This file is for the main operations of the chess engine.
 * It uses all of the other code written to actually make
 * the magic happen.
 */

#include "constants.h"
#include "functions.h"

#include <time.h>

#define WELCOME_STR "Welcome to %s! Type 'console' for console mode, 'uci' for UCI engine mode, or 'xboard' for xboard engine mode. \
Type 'quit' to exit.\n"

/**
 * The function that starts it all.
 */
int main(int argc, char *argv[]) {

  if(argc >= 3) {
    fprintf(stderr, "Too many arguments supplied.\n");
    exit(1);
  }

  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);

  srand(time(NULL));
  init_all();

  bool using_book = false;
  char *bookstr;
  if(argc == 2) {
    // set this to be whatever openings book you want
    // found in the `books` directory.
    bookstr = argv[1];
    using_book = true;
  }

  Board_t board;
  SearchInfo_t info;
  Polybook_t book;

  info.quit = false;
  board.pvt.table = NULL;
  init_hashset(&board.pvt);

  if(using_book) using_book = init_polybook(&book, bookstr);

  printf(WELCOME_STR, NAME);

  char line[BUFSIZ];

  while(true) {
    memset(line, '\0', BUFSIZ);

    fflush(stdout);

    if(!fgets(line, BUFSIZ, stdin)) continue;

    if(*line == '\n') continue;

    if(!strncmp(line, "uci", strlen("uci"))) {
      UCI_loop(&board, &info, book, using_book);
      if(info.quit) break;
      continue;
    } else if(!strncmp(line, "xboard", strlen("xboard"))) {
      XBoard_loop(&board, &info, book, using_book);
      if(info.quit) break;
      continue;
    } else if(!strncmp(line, "console", strlen("console"))) {
      console_loop(&board, &info, book, using_book);
      if(info.quit) break;
      continue;
    } else if(!strncmp(line, "quit", strlen("quit"))) {
      break;
    }
  }


  free(board.pvt.table);
  board.pvt.table = NULL;
  if(using_book) clean_polybook(&book);

  return EXIT_SUCCESS;
}

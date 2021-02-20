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
int main(int argc, char *argv[]) {

  // check argument number
  if(argc >= 3) {
    fprintf(stderr, "Too many arguments supplied.\n");
    exit(1);
  }

  // turn off buffering for stdin and stdout
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);

  // seed the pseudorandom number generator used in polybook.c
  // and initialize
  srand(time(NULL));
  init_all();

  // set up the openings book
  bool using_book = false;
  char *bookstr;
  if(argc == 2) {
    // set this to be whatever openings book you want
    // found in the `books` directory.
    bookstr = argv[1];
    using_book = true;
  }

  // set up or board, searchinfo struct and openings book
  Board_t board;
  SearchInfo_t info;
  Polybook_t book;

  info.quit = false;
  board.pvt.table = NULL;
  init_hashset(&board.pvt); // initialize the principle variation table in the board

  // if we have a book file provided, try to initialize it
  if(using_book) using_book = init_polybook(&book, bookstr);

  printf(WELCOME_STR, NAME);

  // set up input buffer
  char line[BUFSIZ];

  while(true) {
    // this loop pretty much never gets run more than once, but
    // still we should reset the line
    memset(line, '\0', BUFSIZ);

    fflush(stdout);

    // grab user input/GUI input
    if(!fgets(line, BUFSIZ, stdin)) continue;
    if(*line == '\n') continue;

    // select which protocol to use
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


  // clean up any memory we allocated
  free(board.pvt.table);
  board.pvt.table = NULL;
  if(using_book) clean_polybook(&book);

  return EXIT_SUCCESS;
}

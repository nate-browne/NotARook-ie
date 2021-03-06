/**
 * This file contains functions for interacting with the engine via command line
 */

#include "functions.h"
#include "constants.h"

/**
 * Prints the list of commands available when `help` is typed.
 */
static void print_help(void) {
  printf("Commands:\n");
  printf("quit - quit game\n");
  printf("force - computer will not think\n");
  printf("print - show board\n");
  printf("post - show thinking\n");
  printf("nopost - do not show thinking\n");
  printf("new - start new game\n");
  printf("go - set computer thinking\n");
  printf("depth x - set depth to x\n");
  printf("time x - set thinking time to x seconds (depth still applies if set)\n");
  printf("view - show current depth and movetime settings\n");
  printf("** note ** - to reset time and depth, set to 0\n");
  printf("enter moves using UCI notation\n\n\n");
}

/**
 * Loop for interacting with the engine via command line.
 * Protocol is based off of xboard, but without the GUI
 */
void console_loop(Board_t *board, SearchInfo_t *info, Polybook_t book, bool using_book) {
  printf("Booting up %s console mode...\n", NAME);
  printf("Type \"help\" for commands\n\n");

  info->game_mode = CONSOLEMODE;
  info->post_thinking = true;

  // turn off buffering for stdin and stdout
  // we did this back in engine.c, but can't hurt to do it again
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);

  // initialize some stuff we'll be using
  int32_t depth = MAX_DEPTH, movetime = 0;
  enum COLORS engine_side = BLACK;
  uint32_t move = NOMOVE;
  bool res = false;

  // create our input and command buffers
  char in[XBOARD_BUFFER_SIZE], cmd[XBOARD_BUFFER_SIZE];

  // set up the starting position on the board
  parse_FEN(START_FEN, board);

  while(true) {

    fflush(stdout);

    // determine if the engine should be playing a move
    if(board->side == engine_side && !(res = check_result(board))) {
      info->starttime = get_time_millis();
      info->depth = depth;

      if(movetime != 0) {
        info->timeset = true;
        info->stoptime = info->starttime + movetime;
      }

      // find, print, and play the best move the engine can
      search_position(board, info, using_book, book);
    }

    // check if the game is over now after the engine moves
    if(res) {
      print_board(board);
      printf("gg! Type `new` to set up a new game or `quit` to quit.\n");
    }

    // print out the prompt to the screen
    printf("\nNotARook-ie> ");
    fflush(stdout);

    // reset the input buffer
    memset(in, '\0', XBOARD_BUFFER_SIZE);

    // grab the user string and restart the loop if no input provided
    if(!fgets(in, XBOARD_BUFFER_SIZE, stdin)) continue;

    // parse just the command section of the string (no args)
    sscanf(in, "%s", cmd);

    // this one is obvious
    if(!strncmp(cmd, "help", strlen("help"))) {
      print_help();
      continue;
    }

    // this one is obvious too
    if(!strncmp(cmd, "quit", strlen("quit"))) {
      info->quit = true;
      break;
    }

    // engine posts thought process
    if(!strncmp(cmd, "post", strlen("post"))) {
      info->post_thinking = true;
      printf("ok, engine will post thinking output.\n");
      continue;
    }

    // print the board
    if(!strncmp(cmd, "print", strlen("print"))) {
      print_board(board);
      continue;
    }

    // don't post thought process
    if(!strncmp(cmd, "nopost", strlen("nopost"))) {
      info->post_thinking = false;
      printf("ok, engine won't post thinking output\n");
      continue;
    }

    // no thinking for computer
    if(!strncmp(cmd, "force", strlen("force"))) {
      engine_side = BOTH;
      printf("ok, engine will not think.\n");
      continue;
    }

    // report back current config
    if(!strncmp(cmd, "view", strlen("view"))) {
      if(depth == MAX_DEPTH) printf("depth not set ");
      else printf("depth %d ", depth);

      if(movetime != 0) printf(" movetime %ds\n", movetime / 1000);
      else printf(" movetime not set\n");

      continue;
    }

    // enter a depth
    if(!strncmp(cmd, "depth", strlen("depth"))) {
      sscanf(in, "depth %d", &depth);
      if(!depth) depth = MAX_DEPTH;
      printf("ok, setting engine depth to %d\n", depth);
      continue;
    }

    // enter a time control
    if(!strncmp(cmd, "time", strlen("time"))) {
      sscanf(in, "time %d", &movetime);
      movetime *= 1000; // convert to ms
      printf("ok, setting move time to %dms\n", movetime);
      continue;
    }

    // reset the game
    if(!strncmp(cmd, "new", strlen("new"))) {
      engine_side = BLACK;
      parse_FEN(START_FEN, board);
      printf("ok, setting up a new game.\n");
      continue;
    }

    // start the engine
    if(!strncmp(cmd, "go", strlen("go"))) {
      engine_side = board->side;
      continue;
    }

    // if cmd is none of these, assume it is a move
    move = parse_move(in, board);
    if(move == NOMOVE) {
      printf("Unknown command/illegal move: %s\n", in);
      continue;
    }

    // play the user's move
    make_move(board, move);
    board->ply = 0;
  }
}

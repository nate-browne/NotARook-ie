/**
 * This file contains functions pertaining to interacting with GUIs
 * via the UCI protocol. This protocol is stateless; e.g. it turns our engine
 * into a position solver by always telling the engine how to set up the board
 * instead of keeping track of what has happened.
 */

#include "functions.h"
#include "constants.h"

/**
 * Parses the "go" command sent from the GUI.
 * strstr is simpler than strtok in this case (strtok is used in stresstest.c)
 * because tokenizing a whitespace delimited string like the ones this command
 * processes is honestly quite annoying as far as keeping track of what
 * section is for what.
 */
static void parse_go(char *line, SearchInfo_t *info, Board_t *board) {

  // using these initial values to efficiently check if we parsed stuff
  int32_t depth = -1, movestogo = 30, movetime = -1;
  int32_t time = -1, incr = 0;
  char *ptr;
  info->timeset = false;

  if((ptr = strstr(line, "infinite"))) {
    info->timeset = false; // we did this already but better than an empty if block IMO
  }

  /* NOTE: the amount added to ptr is for where that value starts after the phrase found + ONE space */
  /* This is so that we're not "editing" the string at all like we would be with strtok */

  // we have to add an increment for black
  if((ptr = strstr(line, "binc")) && board->side == BLACK) incr = atoi(ptr + 5);

  // we have to add an increment for white
  if((ptr = strstr(line, "winc")) && board->side == WHITE) incr = atoi(ptr + 5);

  // we have to add a max time for black
  if((ptr = strstr(line, "btime")) && board->side == BLACK) time = atoi(ptr + 6);

  // we have to add a max time for white
  if((ptr = strstr(line, "wtime")) && board->side == WHITE) time = atoi(ptr + 6);

  // we have an amount of moves left
  if((ptr = strstr(line, "movestogo"))) movestogo = atoi(ptr + 10);

  // we have a total move time
  if((ptr = strstr(line, "movetime"))) movetime = atoi(ptr + 9);

  // depth is provided
  if((ptr = strstr(line, "depth"))) depth = atoi(ptr + 6);

  // nice hack since if we divide time by movestogo leaves movetime untouched
  if(movetime != -1) {
    time = movetime;
    movestogo = 1;
  }

  info->starttime = get_time_millis();

  // time was set for us
  if(time != -1) {
    info->timeset = true;
    time /= movestogo;
    time -= 50; // remove 50 ms so that we for sure don't go over time
    info->stoptime = info->starttime + time + incr;
  }

  // handle depth
  if(depth == -1) {
    info->depth = MAX_DEPTH;
  } else {
    info->depth = depth;
  }

  printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
    time, info->starttime, info->stoptime, info->depth, info->timeset);
  search_position(board, info);
}

/**
 * Parses a given position in the line read from the GUI
 * This code _does_ assume that the format is consistently correct,
 * so if your GUI isn't correct you'll break this.
 */
static void parse_position(char *line, Board_t *board) {
  line += 9; // jump past "position "
  char *tmp_ptr;

  // we got a restart
  if(!strncmp(line, "startpos", strlen("startpos"))) {
    parse_FEN(START_FEN, board);
  } else {
    // look for the word fen and parse the section after that
    // to get our starting position
    tmp_ptr = strstr(line, "fen");
    if(!tmp_ptr) parse_FEN(START_FEN, board); // failsafe
    else {
      // advance past the "fen " and parse what we see
      tmp_ptr += 4;
      parse_FEN(tmp_ptr, board);
    }
  }

  tmp_ptr = strstr(line, "moves");
  uint32_t move;
  if(tmp_ptr) {
    tmp_ptr += 6; // jump past "moves "
    while(*tmp_ptr) {
      move = parse_move(tmp_ptr, board);
      // if we don't have a valid move break
      // else, play the move
      if(move == NOMOVE) break;

      make_move(board, move);
      board->ply = 0; // make_move changes the ply so reset it (these aren't legit moves)
      // advance to the start of the next move
      while(*tmp_ptr && *tmp_ptr != ' ') tmp_ptr++;
      tmp_ptr++; // once we've hit a space, go to the next for the start of the command
    }
  }

  print_board(board);
}

/**
 * Helper to print out the initialization info required by UCI
 */
static void print_init_info(void) {
  printf("id name %s\n", NAME);
  printf("id author npcompletenate\n");
  printf("uciok\n");
}

/**
 * Loop for interacting with the GUI via the UCI protocol
 */
void UCI_loop(Board_t *board, SearchInfo_t *info) {

  // first, turn off buffering for stdin and stdout
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  info->game_mode = UCIMODE;

  // create the input buffer and let the GUI know we're ready to do battle
  char buf[UCI_BUFFER_SIZE];
  print_init_info();

  // main loop
  while(true) {
    // reset the input buffer and clear stdout
    memset(buf, '\0', UCI_BUFFER_SIZE);
    fflush(stdout);

    if(!fgets(buf, UCI_BUFFER_SIZE, stdin)) continue;

    if(*buf == '\n') continue;

    // case 1: the ready command
    if(!strncmp(buf, "isready", strlen("isready"))) {
      printf("readyok\n");
      continue;
    } else if(!strncmp(buf, "position", strlen("position"))) {
      parse_position(buf, board);
    } else if(!strncmp(buf, "ucinewgame", strlen("ucinewgame"))) {
      parse_position("position startpos\n", board);
    } else if(!strncmp(buf, "go", strlen("go"))) {
      parse_go(buf, info, board);
    } else if(!strncmp(buf, "quit", strlen("quit"))) {
      info->quit = true;
      break;
    } else if(!strncmp(buf, "uci", strlen("uci"))) {
      print_init_info();
    }

    // check for quit if it was sent inside of when we said "go"
    if(info->quit) break;
  }
}

/**
 * This file contains functions pertaining to interacting with GUIs
 * via the xboard protocol. This protocol enforces that the engine
 * keeps track of the state of the game (unlike UCI) so this protocol
 * is closer to an actual game playing engine vs a position solver.
 *
 * See http://www.open-aurec.com/wbforum/WinBoard/engine-intf.html for
 * more information on the gritty details.
 */

#include "functions.h"
#include "constants.h"

/**
 * Check for repetition. Used after every move
 */
static int32_t threefold_repetition(const Board_t *board) {
  int32_t ind = 0, ret = 0;
  for(ind = 0; ind < board->hist_ply; ++ind) {
    if(board->history[ind].hashkey == board->hashkey) ++ret;
  }
  return ret;
}

/**
 * Check if the material on the board is a draw.
 * Doesn't count for theoretical draws (e.g. rook pawn and king vs king where
 * opponent's king is in front of the rook pawn)
 */
static bool draw_material(const Board_t *board) {

  // pawns
  if(board->piece_num[wP] || board->piece_num[bP]) return false;

  // major pieces
  if(board->piece_num[wQ] || board->piece_num[bQ] || board->piece_num[wR] || board->piece_num[bR]) return false;

  // bishops only mate
  if(board->piece_num[wB] > 1 || board->piece_num[bB] > 1) return false;

  // knights only mate
  if(board->piece_num[wN] > 1 || board->piece_num[bN] > 1) return false;

  // knight and bishop mates (lmao I can't do these)
  if(board->piece_num[wN] && board->piece_num[wB]) return false;
  if(board->piece_num[bN] && board->piece_num[bB]) return false;

  // at this point, it's insufficient mating material
  return true;
}

/**
 * Called every time a move is made to check the game for a draw/other results.
 * Any result that ends the game (stalemate/draw/checkmate) returns true
 */
bool check_result(Board_t *board) {

  /* Check for draws first */
  if(board->move_counter > 100) {
    printf("1/2-1/2 {fifty move rule (claimed by NotARook-ie)}\n");
    return true;
  }

  if(threefold_repetition(board) >= 2) {
    printf("1/2-1/2 {3-fold repetition (claimed by NotARook-ie)}\n");
    return true;
  }

  if(draw_material(board)) {
    printf("1/2-1/2 {insufficient mating material (claimed by NotARook-ie)}\n");
    return true;
  }

  // stalemate/checkmate check
  MoveList_t list;
  generate_all_moves(board, &list);

  bool found = false;
  for(int32_t idx = 0; idx < list.count; ++idx) {
    if(!make_move(board, list.moves[idx].move)) continue;

    found = true;
    take_move(board);
    break;
  }

  if(found) return false; // found legal move so not stalemate/checkmate

  bool in_check = square_attacked(board->kings_sq[board->side], board->side ^ 1, board);

  if(in_check) {
    if(board->side == WHITE)  {
      printf("0-1 {black mates (claimed by NotARook-ie)}\n");
      return true;
    } else {
      printf("1-0 {white mates (claimed by NotARook-ie)}\n");
      return true;
    }
  } else {
    printf("1/2-1/2 {stalemate (claimed by NotARook-ie)}\n");
    return true;
  }
}

/**
 * Prints all options available to the GUI.
 * Used when given the "protover 2" command
 */
static void print_options(void) {
  printf("feature ping=1 setboard=1 nps=0 san=0 myname=\"NotARook-ie\" colors=0 usermove=1 sigint=0 sigterm=0 post=1 nopost=1\n");
  printf("feature done=1\n");
}

/**
 * Main loop for interacting with GUIs via the XBoard/Winboard protocol
 * Implementation is inspired by the forum post where the inventor of the protocol
 * wrote a driver for use.
 */
void XBoard_loop(Board_t *board, SearchInfo_t *info, Polybook_t book, bool using_book) {

  // first, turn off buffering for stdin and stdout
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);

  info->game_mode = XBOARDMODE;
  info->post_thinking = true;
  info->initial_time = -1;

  int32_t depth = -1, movestogo[2] = {30, 30}, movetime = -1;
  int32_t time = -1, incr = 0;
  enum COLORS engine_side = BOTH;
  int32_t time_left;
  int32_t sec;
  int32_t mps; // moves per session
  uint32_t move = NOMOVE;

  // set up input and command buffers
  char in[XBOARD_BUFFER_SIZE], cmd[XBOARD_BUFFER_SIZE];

  while(true) {

    fflush(stdout);

    // determine if it is the engine's turn to go
    if(board->side == engine_side && !check_result(board)) {
      info->starttime = get_time_millis();
      info->depth = depth;

      if(time != -1) {
        info->timeset = true;
        time /= movestogo[board->side];
        time -= 50; // stay within the time
        info->stoptime = info->starttime + time + incr;
      }

      if(depth == -1 || depth > MAX_DEPTH) {
        info->depth = MAX_DEPTH;
      }

      // find, report, and play the best move
      printf("time:%d start:%lu stop:%lu depth:%d timeset:%d movestogo:%d mps: %d\n",
        time, info->starttime, info->stoptime, info->depth, info->timeset, movestogo[board->side], mps);
      search_position(board, info, using_book, book);

      if(mps) {
        movestogo[board->side ^ 1]--;
        if(movestogo[board->side ^ 1] < 1) {
          movestogo[board->side ^ 1] = mps;
        }
      }
    }

    fflush(stdout);

    // reset the buffer every time through
    memset(in, '\0', XBOARD_BUFFER_SIZE);
    fflush(stdout);

    // grab the input command
    if(!fgets(in, XBOARD_BUFFER_SIZE, stdin)) continue;

    // replace the newline with a null
    in[strcspn(in, "\n")] = '\0';

    // parse away the command section specificially
    sscanf(in, "%s", cmd);

    // end the loop (and the program)
    if(!strcmp(cmd, "quit")) {
      info->quit = true;
      break;
    }

    // no thonking allowed
    if(!strcmp(cmd, "force")) {
      engine_side = BOTH;
      printf("ok, engine will not think.\n");
      continue;
    }

    // print out the features supported by this engine
    // check the docs to see what kinds of features we could support later
    if(!strcmp(cmd, "protover")) {
      print_options();
      continue;
    }

    // set the search depth
    if(!strcmp(cmd, "sd")) {
      sscanf(in, "sd %d", &depth);
      printf("ok, setting engine depth to %d\n", depth);
      continue;
    }

    // set the max move time for one move
    if(!strcmp(cmd, "st")) {
      sscanf(in, "st %d", &movetime);
      printf("ok, setting move time to %dms\n", movetime);
      continue;
    }

    // time control command for engine
    if(!strcmp(cmd, "time")) {
      sscanf(in, "time %d", &time);
      time *= 10; // time comes in in centiseconds according to the protocol
      printf("ok, setting time for engine to %dms\n", time);
      continue;
    }

    // level command
    // sets up everything to go
    if(!strcmp(cmd, "level")) {
      sec = 0;
      movetime = -1;
      // level command comes in either the top form or bottom form
      // bottom form is because of giving a max amount of seconds to move
      if(sscanf(in, "level %d %d %d", &mps, &time_left, &incr) != 3) {
        sscanf(in, "level %d %d:%d %d", &mps, &time_left, &sec, &incr);
      }
      info->initial_time = time_left;
      time_left *= 60000;
      time_left += sec * 1000;
      movestogo[0] = movestogo[1] = (mps) ? mps : 30;
      time = -1;
      printf("ok, parsed level setup command.\n");
      continue;
    }

    // ping pong
    // print out the value we are "ping"-ed
    if(!strcmp(cmd, "ping")) {
      printf("pong%s\n", in + 4);
      continue;
    }

    // new game
    // set engine to black, reset the depth, reset the board
    if(!strcmp(cmd, "new")) {
      engine_side = BLACK;
      parse_FEN(START_FEN, board);
      depth = -1;
      printf("ok, resetting depth and starting a new game\n");
      continue;
    }

    // command to set up the board with a provided FEN
    if(!strcmp(cmd, "setboard")) {
      engine_side = BOTH;
      parse_FEN(in + 9, board);
      printf("Ok, setting board to FEN pattern %s\n", in + 9);
      continue;
    }

    // tell the engine that it is time to go
    if(!strcmp(cmd, "go")) {
      engine_side = board->side;
      continue;
    }

    // engine posts thought process
    if(!strcmp(cmd, "post")) {
      info->post_thinking = true;
      printf("ok, engine will post thinking output.\n");
      continue;
    }

    // don't post thought process
    if(!strcmp(cmd, "nopost")) {
      info->post_thinking = false;
      printf("ok, engine won't post thinking output\n");
      continue;
    }

    // user made a move, so play it on our board
    if(!strcmp(cmd, "usermove")) {
      movestogo[board->side]--;
      move = parse_move(in + 9, board);
      if(move == NOMOVE) continue;
      make_move(board, move);
      board->ply = 0;
    }
  }
}

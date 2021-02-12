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
 * Main loop for interacting with GUIs via the XBoard/Winboard protocol
 * Implementation is inspired by the forum post where the inventor of the protocol
 * wrote a driver for use.
 */
void XBoard_loop(Board_t *board, SearchInfo_t *info) {

  // first, turn off buffering for stdin and stdout
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);

  info->game_mode = XBOARDMODE;
  info->post_thinking = true;

  int32_t depth = -1, movestogo[2] = {30, 30}, movetime = -1;
  int32_t time = -1, incr = 0;
  int32_t engine_side = BOTH;
  int32_t time_left;
  int32_t sec;
  int32_t mps; // moves per session
  uint32_t move = NOMOVE;
  int32_t i, score;

  char in[XBOARD_BUFFER_SIZE], cmd[XBOARD_BUFFER_SIZE];

  while(true) {

    fflush(stdout);

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

      printf("time:%d start:%d stop:%d depth:%d timeset:%d\n movestogo:%d mps: %d\n",
        time, info->starttime, info->stoptime, info->depth, info->timeset, movestogo[board->side], mps);
      search_position(board, info);

      if(mps) {
        movestogo[board->side ^ 1]--;
        if(movestogo[board->side ^ 1] < 1) {
          movestogo[board->side ^ 1] = mps;
        }
      }
    }

    fflush(stdout);

    memset(in, '\0', XBOARD_BUFFER_SIZE);
    fflush(stdout);
    if(!fgets(in, XBOARD_BUFFER_SIZE, stdin)) continue;

    sscanf(in, "%s", cmd);

    if(!strncmp(cmd, "quit", strlen("quit"))) {
      break;
    }

    // no thonking allowed
    if(!strncmp(cmd, "force", strlen("force"))) {
      engine_side = BOTH;
      continue;
    }

    // print out the features supported by this engine
    // check the docs to see what kinds of features we could support later
    if(!strncmp(cmd, "protover", strlen("protover"))) {
      printf("feature ping=1 setboard=1 colors=0 usermove=1\n");
      printf("feature done=1\n");
      continue;
    }

    // set the search depth
    if(!strncmp(cmd, "sd", strlen("sd"))) {
      sscanf(in, "sd %d", &depth);
      continue;
    }

    // set the max move time for one move
    if(!strncmp(cmd, "st", strlen("st"))) {
      sscanf(in, "st %d", &movetime);
      continue;
    }

    // time control command
    if(!strncmp(cmd, "time", strlen("time"))) {
      sscanf(in, "time %d", &time);
      time *= 10;
      continue;
    }

    // level command
    // sets up everything to go
    if(!strncmp(cmd, "level", strlen("level"))) {
      sec = 0;
      movetime = -1;
      if(sscanf(in, "level %d %d %d", &mps, &time_left, &incr) != 3) {
        sscanf(in, "level %d %d:%d %d", &mps, &time_left, &sec, &incr);
      }
      time_left *= 60000;
      time_left += sec * 1000;
      movestogo[0] = movestogo[1] = (mps) ? mps : 30;
      time = -1;
      continue;
    }

    // ping pong
    // print out the value we are "ping"-ed
    if(!strncmp(cmd, "ping", strlen("ping"))) {
      printf("pong%s\n", in + 4);
      continue;
    }

    // new game
    // set engine to black, reset the depth, reset the board
    if(!strncmp(cmd, "new", strlen("new"))) {
      engine_side = BLACK;
      parse_FEN(START_FEN, board);
      depth = -1;
      continue;
    }

    // command to set up the board with a provided FEN
    if(!strncmp(cmd, "setboard", strlen("setboard"))) {
      engine_side = BOTH;
      parse_FEN(in + 9, board);
      continue;
    }

    // tell the engine that it is time to go
    if(!strncmp(cmd, "go", strlen("go"))) {
      engine_side = board->side;
      continue;
    }

    // user made a move, so play it on our board
    if(!strncmp(cmd, "usermove", strlen("usermove"))) {
      movestogo[board->side]--;
      move = parse_move(in + 9, board);
      if(move == NOMOVE) continue;
      make_move(board, move);
      board->ply = 0;
    }
  }
}

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
char * print_move(const uint32_t move) {

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
 * Parses a move in algebraic notation (e.g. g7g8q for pawn promotion to queen)
 * from standard in
 *
 * Could this have been written better? yes. Do I care? no
 */
uint32_t parse_move(char *str, Board_t *board) {

  // error check
  // yes, this assumes the formatting is very specific but ¯\_(ツ)_/¯
  if(str[1] > '8' || str[1] < '1') return NOMOVE;
  if(str[3] > '8' || str[3] < '1') return NOMOVE;
  if(str[0] > 'h' || str[0] < 'a') return NOMOVE;
  if(str[2] > 'h' || str[2] < 'a') return NOMOVE;

  int32_t from = CONVERT_COORDS(str[0] - 'a', str[1] - '1');
  int32_t to = CONVERT_COORDS(str[2] - 'a', str[3] - '1');

  ASSERT(square_on_board(from) && square_on_board(to));

  MoveList_t list;
  generate_all_moves(board, &list);
  uint32_t move = NOMOVE, promotion_piece = EMPTY;

  for(int32_t ind = 0; ind < list.count; ++ind) {
    move = list.moves[ind].move;

    // if the move matches our generated moves, check for
    // promotion or just return
    if(FROMSQ(move) == from && TOSQ(move) == to) {
      promotion_piece = PROMOTED(move);
      if(promotion_piece != EMPTY) {
        // rook promotion
        if(IsRQ(promotion_piece) && !IsBQ(promotion_piece) && str[4] == 'r') {
          return move;
        // bishop promotion
        } else if(!IsRQ(promotion_piece) && IsBQ(promotion_piece) && str[4] == 'b') {
          return move;
        // queen promotion
        } else if(IsRQ(promotion_piece) && IsBQ(promotion_piece) && str[4] == 'q') {
          return move;
        // knight promotion
        } else if(IsKn(promotion_piece) && str[4] == 'n') {
          return move;
        }
        continue;
      }
      return move;
    }
  }
  return NOMOVE;
}

/**
 * Prints a given move in the standard algebraic notation. Used in console.c
 */
char * print_algebraic_move(const uint32_t move, Board_t *board) {

  static char move_algebraic_str[15];
  char tmp[10];
  memset(tmp, '\0', sizeof(tmp));

  char piece, prom = 'Q';
  bool capture = false;

  bool in_check = false;

  int32_t ff = FILES_BOARD[FROMSQ(move)];
  int32_t rf = RANKS_BOARD[FROMSQ(move)];
  int32_t ft = FILES_BOARD[TOSQ(move)];
  int32_t rt = RANKS_BOARD[TOSQ(move)];

  // handle castles as a special case
  snprintf(tmp, sizeof(tmp), "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));

  // short castles
  if(!strncmp(tmp, "e1g1", strlen("e1g1")) || !strncmp(tmp, "e8g8", strlen("e8g8"))) {
    return "O-O";
  // long castles
  } else if(!strncmp(tmp, "e1c1", strlen("e1c1")) || !strncmp(tmp, "e8c8", strlen("e8c8"))) {
    return "O-O-O";
  }


  int32_t promoted = PROMOTED(move);

  enum PIECES pce = board->pieces[FROMSQ(move)];

  // get the piece type
  switch(pce) {
    case wN:
    case bN:
      piece = 'N';
      break;
    case wR:
    case bR:
      piece = 'R';
      break;
    case wB:
    case bB:
      piece = 'B';
      break;
    case wQ:
    case bQ:
      piece = 'Q';
      break;
    case wK:
    case bK:
      piece = 'K';
      break;
    default: // this means it's a pawn
      piece = '\0';
      break;
  }

  // figure out if capture
  if(move & MFLAGCAP) capture = true;

  // handle all pawn cases
  if(piece == '\0') {
    if(promoted) {
      if(IsKn(promoted)) {
        prom = 'N';
      } else if(IsRQ(promoted) && !IsBQ(promoted)) {
        prom = 'R';
      } else if(!IsRQ(promoted) && IsBQ(promoted)) {
        prom = 'B';
      }
      make_move(board, move);
      in_check = square_attacked(board->kings_sq[board->side], board->side ^ 1, board);
      if(in_check && check_result(board)) {
        if(capture) {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%cx%c%c=%c#", (ff + 'a'), (ft + 'a'), (rt + '1'), prom);
        } else {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%c%c=%c#", (ft + 'a'), (rt + '1'), prom);
        }
      } else if(in_check) {
        if(capture) {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%cx%c%c=%c+", (ff + 'a'), (ft + 'a'), (rt + '1'), prom);
        } else {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%c%c=%c+", (ft + 'a'), (rt + '1'), prom);
        }
      } else {
        if(capture) {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%cx%c%c=%c", (ff + 'a'), (ft + 'a'), (rt + '1'), prom);
        } else {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%c%c=%c", (ft + 'a'), (rt + '1'), prom);
        }
      }
      take_move(board);
    } else {
      make_move(board, move);
      in_check = square_attacked(board->kings_sq[board->side], board->side ^ 1, board);
      if(in_check && check_result(board)) {
        if(capture) {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%cx%c%c#", (ff + 'a'), (ft + 'a'), (rt + '1'));
        } else {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%c%c#", (ft + 'a'), (rt + '1'));
        }
      } else if(in_check) {
        if(capture) {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%cx%c%c+", (ff + 'a'), (ft + 'a'), (rt + '1'));
        } else {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%c%c+", (ft + 'a'), (rt + '1'));
        }
      } else {
        if(capture) {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%cx%c%c", (ff + 'a'), (ft + 'a'), (rt + '1'));
        } else {
          snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%c%c", (ft + 'a'), (rt + '1'));
        }
      }
      take_move(board);
    }
    return move_algebraic_str;
  }

  make_move(board, move);
  in_check = square_attacked(board->kings_sq[board->side], board->side ^ 1, board);
  if(in_check && check_result(board)) {
    if(capture) {
      snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%cx%c%c#", piece, (ft + 'a'), (rt + '1'));
    } else {
      snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%c%c%c#", piece, (ft + 'a'), (rt + '1'));
    }
  } else if(in_check) {
    if(capture) {
      snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%cx%c%c+", piece, (ft + 'a'), (rt + '1'));
    } else {
      snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%c%c%c+", piece, (ft + 'a'), (rt + '1'));
    }
  } else {
    if(capture) {
      snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%cx%c%c", piece, (ft + 'a'), (rt + '1'));
    } else {
      snprintf(move_algebraic_str, sizeof(move_algebraic_str), "%c%c%c", piece, (ft + 'a'), (rt + '1'));
    }
  }
  take_move(board);
  return move_algebraic_str;
}

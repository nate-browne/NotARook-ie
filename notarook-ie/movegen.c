/**
 * This file contains functions for move generation (exciting)
 */

#include "functions.h"
#include "constants.h"

/**
 * Moves not involving captures
 */
static void add_quiet_move(const Board_t *board, int32_t move, MoveList_t *list) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

/**
 * ...you can guess what this does
 */
static void add_capture_move(const Board_t *board, int32_t move, MoveList_t *list) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

/**
 * this one too
 */ 
static void add_enpassant_move(const Board_t *board, int32_t move, MoveList_t *list) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

/**
 * ...I mean it's self explanatory no?
 */
static void add_white_pawn_capture(const Board_t *board, const int32_t from, const int32_t to, const int32_t cap, MoveList_t *list) {
  // this means we're promoting the pawn
  if(RANKS_BOARD[from] == RANK_7) {
    add_capture_move(board, MOVE(from,to,cap,wQ,0), list);
    add_capture_move(board, MOVE(from,to,cap,wR,0), list);
    add_capture_move(board, MOVE(from,to,cap,wB,0), list);
    add_capture_move(board, MOVE(from,to,cap,wN,0), list);
  } else {
    add_capture_move(board, MOVE(from,to,cap,EMPTY,0), list);
  }
}

/**
 * ...I mean it's self explanatory no?
 */
static void add_black_pawn_capture(const Board_t *board, const int32_t from, const int32_t to, const int32_t cap, MoveList_t *list) {
  // this means we're promoting the pawn
  if(RANKS_BOARD[from] == RANK_2) {
    add_capture_move(board, MOVE(from,to,cap,bQ,0), list);
    add_capture_move(board, MOVE(from,to,cap,bR,0), list);
    add_capture_move(board, MOVE(from,to,cap,bB,0), list);
    add_capture_move(board, MOVE(from,to,cap,bN,0), list);
  } else {
    add_capture_move(board, MOVE(from,to,cap,EMPTY,0), list);
  }
}

/**
 * I should stop adding headers to these functions...
 */
static void add_white_pawn_move(const Board_t *board, const int32_t from, const int32_t to, MoveList_t *list) {
  // this means we're promoting the pawn
  if(RANKS_BOARD[from] == RANK_7) {
    add_capture_move(board, MOVE(from,to,EMPTY,wQ,0), list);
    add_capture_move(board, MOVE(from,to,EMPTY,wR,0), list);
    add_capture_move(board, MOVE(from,to,EMPTY,wB,0), list);
    add_capture_move(board, MOVE(from,to,EMPTY,wN,0), list);
  } else {
    add_capture_move(board, MOVE(from,to,EMPTY,EMPTY,0), list);
  }
}

/**
 * I should stop adding headers to these functions...
 */
static void add_black_pawn_move(const Board_t *board, const int32_t from, const int32_t to, MoveList_t *list) {
  // this means we're promoting the pawn
  if(RANKS_BOARD[from] == RANK_2) {
    add_capture_move(board, MOVE(from,to,EMPTY,bQ,0), list);
    add_capture_move(board, MOVE(from,to,EMPTY,bR,0), list);
    add_capture_move(board, MOVE(from,to,EMPTY,bB,0), list);
    add_capture_move(board, MOVE(from,to,EMPTY,bN,0), list);
  } else {
    add_capture_move(board, MOVE(from,to,EMPTY,EMPTY,0), list);
  }
}

/**
 * Generate all moves for a given board position
 */
void generate_all_moves(const Board_t *board, MoveList_t *list) {

  check_board(board);

  list->count = 0;

  int32_t piece = EMPTY;
  int32_t side = board->side;
  int32_t sq = 0, temp_sq = 0;
  int32_t piece_num = 0;

  if(side == WHITE) {
    for(piece_num = 0; piece_num < board->piece_num[wP]; ++piece_num) {
      sq = board->piece_list[wP][piece_num];

      ASSERT(square_on_board(sq));

      // in our representation, forward for white is + 10 coordinate points
      if(board->pieces[sq + 10] == EMPTY) {
        add_white_pawn_move(board, sq, sq + 10, list);
        if(RANKS_BOARD[sq] == RANK_2 && board->pieces[sq + 20] == EMPTY) {
          add_quiet_move(board, MOVE(sq, sq + 20, EMPTY, EMPTY, MFLAGPS), list);
        }
      }

      // checking for white's pawn captures
      if(!SQOFFBOARD(sq + 9) && PIECE_COL[board->pieces[sq + 9]] == BLACK) {
        add_white_pawn_capture(board, sq, sq + 9, board->pieces[sq + 9], list);
      }
      if(!SQOFFBOARD(sq + 11) && PIECE_COL[board->pieces[sq + 11]] == BLACK) {
        add_white_pawn_capture(board, sq, sq + 11, board->pieces[sq + 11], list);
      }

      // check on croissant move
      if(sq + 9 == board->passant) {
        add_capture_move(board, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
      }
      if(sq + 11 == board->passant) {
        add_capture_move(board, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
      }
    }
  } else {
    for(piece_num = 0; piece_num < board->piece_num[bP]; ++piece_num) {
      sq = board->piece_list[bP][piece_num];

      ASSERT(square_on_board(sq));

      // in our representation, forward for black is - 10 coordinate points
      if(board->pieces[sq - 10] == EMPTY) {
        add_black_pawn_move(board, sq, sq - 10, list);
        if(RANKS_BOARD[sq] == RANK_7 && board->pieces[sq - 20] == EMPTY) {
          add_quiet_move(board, MOVE(sq, sq - 20, EMPTY, EMPTY, MFLAGPS), list);
        }
      }

      // checking for blacks's pawn captures
      if(!SQOFFBOARD(sq - 9) && PIECE_COL[board->pieces[sq - 9]] == WHITE) {
        add_black_pawn_capture(board, sq, sq - 9, board->pieces[sq - 9], list);
      }
      if(!SQOFFBOARD(sq - 11) && PIECE_COL[board->pieces[sq - 11]] == WHITE) {
        add_black_pawn_capture(board, sq, sq - 11, board->pieces[sq - 11], list);
      }

      // check on croissant move
      if(sq - 9 == board->passant) {
        add_capture_move(board, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
      }
      if(sq - 11 == board->passant) {
        add_capture_move(board, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
      }
    }
  }
}

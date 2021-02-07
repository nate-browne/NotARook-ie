/**
 * This file contains functions for move generation (exciting)
 */

#include "functions.h"
#include "constants.h"

// the 0s give us a way to disambiguate
// between white and black
static int32_t LOOP_SLIDE_PIECE[8] = {
  wB, wR, wQ, 0, bB, bR, bQ, 0
};

// the 0s give us a way to disambiguate
// between white and black
static int32_t LOOP_NON_SLIDE_PIECE[6] = {
  wN, wK, 0, bN, bK, 0
};

// black to move -> 4, white to move -> 0
// helps us out with the above array to condense the
// code and not end up with the same mess as the pawns
static int32_t LOOP_SLIDE_INDEX[2] = {0, 4};

// same reasoning as above array
static int32_t LOOP_NON_SLIDE_INDEX[2] = {0, 3};

// indexing by piece
static int32_t NUM_DIR[13] = {
  0,0,8,4,4,8,8,0,8,4,4,8,8
};

// the directions on the coordinate system each piece can move
// maps to the above NUM_DIR
static int32_t PIECE_DIR[13][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {-8,-19,-21,-12,8,19,21,12},
  {-9,-11,11,9,0,0,0,0},
  {-1,-10,1,10,0,0,0,0},
  {-1,-10,1,10,-9,-11,11,9},
  {-1,-10,1,10,-9,-11,11,9},
  {0,0,0,0,0,0,0,0},
  {-8,-19,-21,-12,8,19,21,12},
  {-9,-11,11,9,0,0,0,0},
  {-1,-10,1,10,0,0,0,0},
  {-1,-10,1,10,-9,-11,11,9},
  {-1,-10,1,10,-9,-11,11,9}
};

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

  ASSERT(piece_valid_empty(cap));
  ASSERT(square_on_board(from));
  ASSERT(square_on_board(to));

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

  ASSERT(piece_valid_empty(cap));
  ASSERT(square_on_board(from));
  ASSERT(square_on_board(to));

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

  ASSERT(square_on_board(from));
  ASSERT(square_on_board(to));

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

  ASSERT(square_on_board(from));
  ASSERT(square_on_board(to));

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
  int32_t index = 0;
  int32_t dir;

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

    // castling, first kingside
    // if we can castle, the squares are empty, and the squares aren't attacked, we may
    // be able to castle. We don't check for if the king is moving through check here
    // since we do that later in the function
    if(board->castle_permission & WKCAS) {
      if(board->pieces[F1] == EMPTY && board->pieces[G1] == EMPTY) {
        if(!square_attacked(E1, BLACK, board) && !square_attacked(F1, BLACK, board)) {
          printf("WKCA MoveGen\n");
        }
      }
    }

    // logic here is very much the same as kingside above
    if(board->castle_permission & WQCAS) {
      if(board->pieces[D1] == EMPTY && board->pieces[C1] == EMPTY && board->pieces[B1] == EMPTY) {
        if(!square_attacked(E1, BLACK, board) && !square_attacked(D1, BLACK, board)) {
          printf("WQCA MoveGen\n");
        }
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

    // castling, first kingside
    // if we can castle, the squares are empty, and the squares aren't attacked, we may
    // be able to castle. We don't check for if the king is moving through check here
    // since we do that later in the function
    if(board->castle_permission & BKCAS) {
      if(board->pieces[F8] == EMPTY && board->pieces[G8] == EMPTY) {
        if(!square_attacked(E8, WHITE, board) && !square_attacked(F8, WHITE, board)) {
          printf("BKCA MoveGen\n");
        }
      }
    }

    // logic here is very much the same as kingside above
    if(board->castle_permission & BQCAS) {
      if(board->pieces[D8] == EMPTY && board->pieces[C8] == EMPTY && board->pieces[B8] == EMPTY) {
        if(!square_attacked(E8, WHITE, board) && !square_attacked(D8, WHITE, board)) {
          printf("BQCA MoveGen\n");
        }
      }
    }
  }

  // sllllllliiiide to the left (and right, front, back, and diagonally)
  int32_t piece_index = LOOP_SLIDE_INDEX[side];
  piece = LOOP_SLIDE_PIECE[piece_index++];

  while(piece) {
    ASSERT(piece_valid(piece));

    for(piece_num = 0; piece_num < board->piece_num[piece]; ++piece_num) {
      sq = board->piece_list[piece][piece_num];

      ASSERT(square_on_board(sq));
      printf("Piece:%c on %s\n", PIECE_CHAR[piece], print_square(sq));

      for(index = 0; index < NUM_DIR[piece]; ++index) {
        dir = PIECE_DIR[piece][index];
        temp_sq = sq + dir;

        while(!SQOFFBOARD(temp_sq)) {
          if(board->pieces[temp_sq] != EMPTY) {
            if(PIECE_COL[board->pieces[temp_sq]] == (side ^ 1)) {
              printf("\t\tCapture on %s\n", print_square(temp_sq));
            }
            break;
          }
          printf("\t\tNormal on %s\n", print_square(temp_sq));
          temp_sq += dir; // increment the temp square by the direction to go to the next
                          // square
        }
      }
    }

    piece = LOOP_SLIDE_PIECE[piece_index++];
  }

  // non sliding pieces
  piece_index = LOOP_NON_SLIDE_INDEX[side];
  piece = LOOP_NON_SLIDE_PIECE[piece_index++];
  while(piece) {
    ASSERT(piece_valid(piece));

    for(piece_num = 0; piece_num < board->piece_num[piece]; ++piece_num) {
      sq = board->piece_list[piece][piece_num];
      ASSERT(square_on_board(sq));
      printf("Piece:%c on %s\n", PIECE_CHAR[piece], print_square(sq));

      for(index = 0; index < NUM_DIR[piece]; ++index) {
        dir = PIECE_DIR[piece][index];
        temp_sq = sq + dir;

        if(!SQOFFBOARD(temp_sq)) {

          // captures
          // little bit of bit magic where WHITE ^ 1 == BLACK and vice versa
          if(board->pieces[temp_sq] != EMPTY) {
            if(PIECE_COL[board->pieces[temp_sq]] == (side ^ 1)) {
              printf("\t\tCapture on %s\n", print_square(temp_sq));
            }
            continue;
          }
          printf("\t\tNormal on %s\n", print_square(temp_sq));
        } else {
          continue;
        }
      }
    }
    piece = LOOP_NON_SLIDE_PIECE[piece_index++];
  }
}

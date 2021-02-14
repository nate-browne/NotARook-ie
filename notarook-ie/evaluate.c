/**
 * File containing functions pertaining to move evaluation and selection
 */

#include "constants.h"
#include "functions.h"

static const int32_t PAWN_TABLE[STANDARD_BOARD_SIZE] = {
0,0,0,0,0,0,0,0,
10 , 10 , 0 , -10 , -10 , 0 , 10 , 10 ,
5 , 0 , 0 , 5 , 5 , 0 , 0 , 5 ,
0 , 0 , 10 , 20 , 20 , 10 , 0 , 0 ,
5 , 5 , 5 , 10 , 10 , 5 , 5 , 5 ,
10 , 10 , 10 , 20 , 20 , 10 , 10 , 10 ,
20 , 20 , 20 , 30 , 30 , 20 , 20 , 20 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 
};

static const int32_t KNIGHT_TABLE[STANDARD_BOARD_SIZE] = {
0 , -10 , 0 , 0 , 0 , 0 , -10 , 0 ,
0 , 0 , 0 , 5 , 5 , 0 , 0 , 0 ,
0 , 0 , 10 , 10 , 10 , 10 , 0 , 0 ,
0 , 5 , 10 , 20 , 20 , 10 , 5 , 0 ,
5 , 10 , 15 , 20 , 20 , 15 , 10 , 5 ,
5 , 10 , 10 , 20 , 20 , 10 , 10 , 5 ,
0 , 0 , 5 , 10 , 10 , 5 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  
};

static const int32_t BISHOP_TABLE[STANDARD_BOARD_SIZE] = {
0 , 0 , -10 , 0 , 0 , -10 , 0 , 0 ,
0 , 0 , 0 , 10 , 10 , 0 , 0 , 0 ,
0 , 0 , 10 , 15 , 15 , 10 , 0 , 0 ,
0 , 10 , 15 , 20 , 20 , 15 , 10 , 0 ,
0 , 10 , 15 , 20 , 20 , 15 , 10 , 0 ,
0 , 0 , 10 , 15 , 15 , 10 , 0 , 0 ,
0 , 0 , 0 , 10 , 10 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 
};

static const int32_t ROOK_TABLE[STANDARD_BOARD_SIZE] = {
0 , 0 , 5 , 10 , 10 , 5 , 0 , 0 ,
0 , 0 , 5 , 10 , 10 , 5 , 0 , 0 ,
0 , 0 , 5 , 10 , 10 , 5 , 0 , 0 ,
0 , 0 , 5 , 10 , 10 , 5 , 0 , 0 ,
0 , 0 , 5 , 10 , 10 , 5 , 0 , 0 ,
0 , 0 , 5 , 10 , 10 , 5 , 0 , 0 ,
25 , 25 , 25 , 25 , 25 , 25 , 25 , 25 ,
0 , 0 , 5 , 10 , 10 , 5 , 0 , 0  
};

// evaluation table for king in the end game
static const int32_t KING_E[STANDARD_BOARD_SIZE] = {
 -50 , -10 , 0 , 0 , 0 , 0 , -10 , -50 ,
 -10, 0 , 10 , 10 , 10 , 10 , 0 , -10 ,
 0 , 10 , 15 , 15 , 15 , 15 , 10 , 0 ,
 0 , 10 , 15 , 20 , 20 , 15 , 10 , 0 ,
 0 , 10 , 15 , 20 , 20 , 15 , 10 , 0 ,
 0 , 10 , 15 , 15 , 15 , 15 , 10 , 0 ,
 -10, 0 , 10 , 10 , 10 , 10 , 0 , -10 ,
 -50 , -10 , 0 , 0 , 0 , 0 , -10 , -50 ,
};

// evaluation table for kings in the opening
// tries to encourage castling
static const int32_t KING_O[STANDARD_BOARD_SIZE] = {
 0 , 5 , 5 , -10 , -10 , 0 , 10 , 5 ,
 -10 , -10 , -10 , -10 , -10 , -10 , -10 , -10 ,
 -30 , -30 , -30 , -30 , -30 , -30 , -30 , -30 ,
 -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70 ,
 -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70 ,
 -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70 ,
 -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70 ,
 -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70  
};

// have a negative reward for isolated pawns to discourage the
// engine from leaving these
static const int32_t pawn_isolated = -10;

// give bonuses for passed pawns depending on which rank they're on
static const int32_t PAWN_PASSED[8] = {0, 5, 10, 20, 35, 60, 100, 200};

// give a bonus for putting the rook on an open file
static const int32_t rook_open_file = 10;

// give a smaller bonus for putting a rook on a semiopen file
static const int32_t rook_semiopen_file = 5;

// give a bonus for putting the rook on an open file
// queen gets lower values since it is able to be used
// on more than just files
static const int32_t queen_open_file = 5;

// give a smaller bonus for putting a rook on a semiopen file
static const int32_t queen_semiopen_file = 3;

// give a decently large bonus for keeping both bishops
static const int32_t bishop_pair = 30;

/**
 * This function figures out whether or not the game is a draw by material
 * Taken from sjeng engine version 11.2
 */
static bool material_draw(const Board_t *board) {
  // first, check for rooks and queens
  if(!board->piece_num[wR] && !board->piece_num[bR] && !board->piece_num[wQ] && !board->piece_num[bQ]) {
    // check for bishops
    if(!board->piece_num[bB] && !board->piece_num[wB]) {
      // just knights is a material draw
      if(board->piece_num[wN] < 3 && board->piece_num[bN] < 3) {
        return true;
      }
      // no knights
    } else if(!board->piece_num[wN] && !board->piece_num[bN]) {
      if(abs(board->piece_num[wB] - board->piece_num[bB]) < 2) {
        return true;
      }
    } else if((board->piece_num[wN] < 3 && !board->piece_num[wB]) || (board->piece_num[wB] == 1 && !board->piece_num[wN])) {
      if((board->piece_num[bN] < 3 && !board->piece_num[bB]) || (board->piece_num[bB] == 1 && !board->piece_num[bN])) {
        return true;
      }
    }
  // no queens on the board but we have rooks
  } else if(!board->piece_num[wQ] && !board->piece_num[bQ]) {
    // scenario 1: white and black each have one rook
    if(board->piece_num[wR] == 1 && board->piece_num[bR] == 1) {
      if((board->piece_num[wN] + board->piece_num[wB] < 2) && (board->piece_num[bN] + board->piece_num[bB] < 2)) {
        return true;
      }
    // scenario 2: white has a rook, black doesn't
    } else if(board->piece_num[wR] == 1 && !board->piece_num[bR]) {
      if((board->piece_num[wN] + board->piece_num[wB] == 0) && (((board->piece_num[bN] + board->piece_num[bB]) == 1) || ((board->piece_num[bN] + board->piece_num[bB]) == 2))) {
        return true;
      }
    // scenario 3: black has a rook, white doesn't
    } else if(board->piece_num[bR] == 1 && !board->piece_num[wR]) {
      if((board->piece_num[bN] + board->piece_num[bB] == 0) && (((board->piece_num[wN] + board->piece_num[wB]) == 1) || ((board->piece_num[wN] + board->piece_num[wB]) == 2))) {
        return true;
      }
    }
  }
  return false;
}

/**
 * This function takes in a position on the board
 * and returns the evaluation of the position in
 * 100s of a pawn from the point of view of the side to move
 */
int32_t eval_position(const Board_t * board) {

  int32_t piece, square;

  // white == positive, black == negative
  // we negate at the end if black is to move
  int32_t score = board->material[WHITE] - board->material[BLACK];

  // if our position is drawn purely by material, return so
  if(!board->piece_num[wP] && !board->piece_num[bP] && material_draw(board)) return 0;

  // white pawns first
  piece = wP;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score += PAWN_TABLE[SQ64(square)];

    // check for isolated pawns and deduct from the score if we have them
    if(!(ISOLATED_MASK[SQ64(square)] & board->pawns[WHITE])) {
      score += pawn_isolated;
    }

    // check for black passed pawns and deduct from our score if we have them
    // we bitwise AND with the opposite color because if it's 0, no pawns
    // of white block blacks's path
    if(!(WHITE_PASSED_MASK[SQ64(square)] & board->pawns[BLACK])) {
      score += PAWN_PASSED[RANKS_BOARD[square]];
    }
  }

  // black pawns next
  piece = bP;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score -= PAWN_TABLE[MIRROR64(SQ64(square))];

    // check for isolated pawns and add to the score if we have them
    if(!(ISOLATED_MASK[SQ64(square)] & board->pawns[BLACK])) {
      score -= pawn_isolated;
    }

    // check for white passed pawns and add from our score if we have them
    // we bitwise AND with the opposite color because if it's 0, no pawns
    // of black block white's path
    if(!(BLACK_PASSED_MASK[SQ64(square)] & board->pawns[WHITE])) {
      // scores are indexed by rank so we have to subtract to make sure our scores get more positive
      // as we advance up the board
      score -= PAWN_PASSED[7 - RANKS_BOARD[square]];
    }
  }

  // r/whiteknighting
  piece = wN;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score += KNIGHT_TABLE[SQ64(square)];
  }

  // black knights
  piece = bN;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score -= KNIGHT_TABLE[MIRROR64(SQ64(square))];
  }

  // white bishops
  piece = wB;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score += BISHOP_TABLE[SQ64(square)];
  }

  // black bishops
  piece = bB;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score -= BISHOP_TABLE[MIRROR64(SQ64(square))];
  }

  // white rooks
  piece = wR;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score += ROOK_TABLE[SQ64(square)];

    // check for open files
    if(!(board->pawns[BOTH] & FILE_BB_MASK[FILES_BOARD[square]])) {
      score += rook_open_file;
    // if the file is blocked by the opposite color pawn, it's semiopen
    } else if(!(board->pawns[WHITE] & FILE_BB_MASK[FILES_BOARD[square]])) {
      score += rook_semiopen_file;
    }
  }

  // black rooks
  piece = bR;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score -= ROOK_TABLE[MIRROR64(SQ64(square))];

    // check for open files
    if(!(board->pawns[BOTH] & FILE_BB_MASK[FILES_BOARD[square]])) {
      score -= rook_open_file;
    // if the file is blocked by the opposite color pawn, it's semiopen
    } else if(!(board->pawns[BLACK] & FILE_BB_MASK[FILES_BOARD[square]])) {
      score -= rook_semiopen_file;
    }
  }

  // white queen
  piece = wQ;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));

    // check for open files
    if(!(board->pawns[BOTH] & FILE_BB_MASK[FILES_BOARD[square]])) {
      score += queen_open_file;
    // if the file is blocked by the opposite color pawn, it's semiopen
    } else if(!(board->pawns[WHITE] & FILE_BB_MASK[FILES_BOARD[square]])) {
      score += queen_semiopen_file;
    }
  }

  // black queen
  piece = bQ;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));

    // check for open files
    if(!(board->pawns[BOTH] & FILE_BB_MASK[FILES_BOARD[square]])) {
      score -= queen_open_file;
    // if the file is blocked by the opposite color pawn, it's semiopen
    } else if(!(board->pawns[BLACK] & FILE_BB_MASK[FILES_BOARD[square]])) {
      score -= queen_semiopen_file;
    }
  }

  // handle kings
  piece = wK;
  square = board->piece_list[piece][WHITE];

  // check if we should use the endgame table or the normal table
  if(board->material[BLACK] <= ENDGAME_MAT) {
    score += KING_E[SQ64(square)];
  } else {
    score += KING_O[SQ64(square)];
  }

  piece = bK;
  square = board->piece_list[piece][BLACK];

  // check if we should use the endgame table or the normal table
  if(board->material[WHITE] <= ENDGAME_MAT) {
    score -= KING_E[MIRROR64(SQ64(square))];
  } else {
    score -= KING_O[MIRROR64(SQ64(square))];
  }

  // check for bishop pairs
  if(board->piece_num[wB] >= 2) score += bishop_pair;
  if(board->piece_num[bB] >= 2) score -= bishop_pair;

  return (board->side == WHITE) ? score : -score;
}

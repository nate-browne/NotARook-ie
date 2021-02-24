/**
 * This file has functions to deal with answering questions such as
 * "is <insert square here> attacked?"
 */

#include "constants.h"
#include "functions.h"

/**
 * Takes in the square in question, the attacking side, and the board
 * figures out if the given square is being attacke
 */
bool square_attacked(const int32_t square, const int32_t side, const Board_t *board) {

  ASSERT(square_on_board(square));
  ASSERT(side_valid(side));
  ASSERT(check_board(board));

  int32_t piece, ind, temp_square, dir;

  // pawns
  // the subtraction is for going back diagonally, addition for up diagonally
  if(side == WHITE) {
    if(board->pieces[square - 11] == wP || board->pieces[square - 9] == wP) return true;
  } else {
    if(board->pieces[square + 11] == bP || board->pieces[square + 9] == bP) return true;
  }

  // knights
  for(ind = 0; ind < (int32_t)(sizeof(KNIGHT_DIRS) / sizeof(*KNIGHT_DIRS)); ++ind) {
    piece = board->pieces[square + KNIGHT_DIRS[ind]];
    if(piece != EMPTY && piece != OFFBOARD && IsKn(piece) && PIECE_COL[piece] == side)
      return true;
  }

  // rooks and queens
  for(ind = 0; ind < (int32_t)(sizeof(ROOK_DIRS) / sizeof(*ROOK_DIRS)); ++ind) {
    dir = ROOK_DIRS[ind];
    temp_square = square + dir;
    piece = board->pieces[temp_square];

    while(piece != OFFBOARD) {
      if(piece != EMPTY) {
        if(IsRQ(piece) && PIECE_COL[piece] == side) {
          return true;
        }
        break;
      }
      temp_square += dir;
      piece = board->pieces[temp_square];
    }
  }

  // bishops and queens
  for(ind = 0; ind < (int32_t)(sizeof(BISHOP_DIRS) / sizeof(*BISHOP_DIRS)); ++ind) {
    dir = BISHOP_DIRS[ind];
    temp_square = square + dir;
    piece = board->pieces[temp_square];

    while(piece != OFFBOARD) {
      if(piece != EMPTY) {
        if(IsBQ(piece) && PIECE_COL[piece] == side) {
          return true;
        }
        break;
      }
      temp_square += dir;
      piece = board->pieces[temp_square];
    }
  }

  // kings
  for(ind = 0; ind < (int32_t)(sizeof(KING_DIRS) / sizeof(*KING_DIRS)); ++ind) {
    piece = board->pieces[square + KING_DIRS[ind]];
    if(piece != OFFBOARD && piece != EMPTY && IsKi(piece) && PIECE_COL[piece] == side)
      return true;
  }

  return false;
}

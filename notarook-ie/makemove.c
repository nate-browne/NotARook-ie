/**
 * This file has the functions that have to do with
 * making a move/undo-ing a move
 */

#include "functions.h"
#include "constants.h"

// this array helps for shorthand references to
// whether or not we can castle.
// each space that isn't 15 is for if a rook or king moves
// (which removes the ability to castle)
static const int32_t CASTLE_PERM[120] = {
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
  0xF, 0xD, 0xF, 0xF, 0xF, 0xC, 0xF, 0xF, 0xE, 0xF, 
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
  0xF, 0x7, 0xF, 0xF, 0xF, 0x3, 0xF, 0xF, 0xB, 0xF, 
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
  0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 
};

/**
 * Helper function to clear a given square of the given board of
 * whatever piece may be on it.
 */
static void clear_piece(const int32_t sq, Board_t *board) {

  // grab values and verify
  ASSERT(square_on_board(sq));

  int32_t piece = board->pieces[sq];

  ASSERT(piece_valid(piece));

  int32_t col = PIECE_COL[piece], ind = 0, target_piece_num = -1;

  // Hash the value into the key, clear the square, reduce material value
  HASH_PIECE(piece, sq);
  board->pieces[sq] = EMPTY;
  board->material[col] -= PIECE_VAL[piece];

  // if it's anything other than a pawn, decrease those counts
  if(PIECE_BIG[piece]) {
    board->big_pieces[col]--;

    if(PIECE_MAJ[piece]) board->maj_pieces[col]--;
    else board->min_pieces[col]--;
  } else {

    // pawns are kept on the bitboard as well, so we need to clear those values
    // if applicable
    CLRBIT(board->pawns[col], SQ64(sq));
    CLRBIT(board->pawns[BOTH], SQ64(sq));
  }

  // remove the piece from the piece list of the board
  for(ind = 0; ind < board->piece_num[piece]; ++ind) {

    // if the piece in the list for this index is on our square,
    // we found the piece to remove
    if(board->piece_list[piece][ind] == sq) {
      target_piece_num = ind;
      break;
    }
  }

  // should have found a piece
  ASSERT(target_piece_num != -1);

  // we're just overwriting values, effectively "removing" it
  board->piece_num[piece]--;
  board->piece_list[piece][target_piece_num] = board->piece_list[piece][board->piece_num[piece]];
}

/**
 * Helper function to add a piece to the given board
 */
static void add_piece(const int32_t sq, Board_t *board, const int32_t piece) {

  ASSERT(piece_valid(piece));
  ASSERT(square_on_board(sq));

  int32_t col = PIECE_COL[piece];

  HASH_PIECE(piece, sq);

  board->pieces[sq] = piece;

  // if it's anything other than a pawn, increase those counts
  if(PIECE_BIG[piece]) {
    board->big_pieces[col]++;

    if(PIECE_MAJ[piece]) board->maj_pieces[col]++;
    else board->min_pieces[col]++;
  } else {

    // pawns are kept on the bitboard as well, so we need to increase those values
    SETBIT(board->pawns[col], SQ64(sq));
    SETBIT(board->pawns[BOTH], SQ64(sq));
  }

  board->material[col] += PIECE_VAL[piece];
  board->piece_list[piece][board->piece_num[piece]++] = sq;
}

/**
 * Helper to move a piece on the given board
 */
static void move_piece(const int32_t from, const int32_t to, Board_t *board) {

  ASSERT(square_on_board(from));
  ASSERT(square_on_board(to));

  int32_t ind = 0;
  int32_t piece = board->pieces[from];
  int32_t col = PIECE_COL[piece];

  bool found = false;

  HASH_PIECE(piece, from);
  board->pieces[from] = EMPTY;

  HASH_PIECE(piece, to);
  board->pieces[to] = piece;

  // pawns also go on the bitboard
  if(!PIECE_BIG[piece]) {
    CLRBIT(board->pawns[col], SQ64(from));
    CLRBIT(board->pawns[BOTH], SQ64(from));
    SETBIT(board->pawns[col], SQ64(to));
    SETBIT(board->pawns[BOTH], SQ64(to));
  }

  // piece list setting
  for(ind = 0; ind < board->piece_num[piece]; ++ind) {
    if(board->piece_list[piece][ind] == from) {
      board->piece_list[piece][ind] = to;

      found = true;
      break;
    }
  }

  ASSERT(found);
}


/**
 * Function to undo a move made by the below function
 */
void take_move(Board_t *board) {
  check_board(board);

  // decrement the ply values
  board->hist_ply--;
  board->ply--;

  // grab the move before our illegal move
  int32_t move = board->history[board->hist_ply].move_played;
  int32_t from = FROMSQ(move);
  int32_t to = TOSQ(move);

  // just for sanity checking
  ASSERT(square_on_board(from));
  ASSERT(square_on_board(to));

  // remove the on croissant and castle from the hash
  if(board->passant != NO_SQ) HASH_PAS;
  HASH_CAS;

  // grab the castle permissions, move count, and en passant square from history
  board->castle_permission = board->history[board->hist_ply].castle_permission;
  board->move_counter = board->history[board->hist_ply].move_counter;
  board->passant = board->history[board->hist_ply].passant;

  // hash it back in if the field is now set
  if(board->passant != NO_SQ) HASH_PAS;
  HASH_CAS;

  // flip side back and hash
  board->side ^= 1;
  HASH_SIDE;

  if(move & MFLAGEP) {
    if(board->side == WHITE) add_piece(to - 10, board, bP);
    else add_piece(to + 10, board, wP);
  } else if(move & MFLAGCAS) {
    switch(to) {
      case C1:
        move_piece(D1, A1, board);
        break;
      case C8:
        move_piece(D8, A8, board);
        break;
      case G1:
        move_piece(F1, H1, board);
        break;
      case G8:
        move_piece(F8, H8, board);
        break;
      default:
        ASSERT(false); //illegal castle
    }
  }

  move_piece(to, from, board);

  // revert any king moves
  if(PIECE_KING[board->pieces[from]]) {
    board->kings_sq[board->side] = from;
  }

  // revert the capture
  int32_t captured = CAPTURED(move);
  if(captured != EMPTY) {
    ASSERT(piece_valid(captured));
    add_piece(to, board, captured);
  }

  // revert promotions
  if(PROMOTED(move) != EMPTY) {
    ASSERT(piece_valid(PROMOTED(move)) && !PIECE_PAWN[PROMOTED(move)]);
    clear_piece(from, board);
    add_piece(from, board, (PIECE_COL[PROMOTED(move)] == WHITE ? wP : bP));
  }

  check_board(board);
}

/**
 * Time to make a move!
 * Returns false if the side that made the move is in
 * check afterwards
 */
bool make_move(Board_t *board, int32_t move) {

  check_board(board);

  int32_t from = FROMSQ(move);
  int32_t to = TOSQ(move);
  int32_t side = board->side;

  ASSERT(square_on_board(from));
  ASSERT(square_on_board(to));
  ASSERT(side_valid(side));
  ASSERT(piece_valid(board->pieces[from]));

  // store the current board state in the history
  board->history[board->hist_ply].hashkey = board->hashkey;

  /* handle special moves */

  // on croissant first
  if(move & MFLAGEP) {
    if(side == WHITE) clear_piece(to - 10, board);
    else clear_piece(to + 10, board);

  // castling next
  } else if(move & MFLAGCAS) {
    switch(to) {
      case C1:
        move_piece(A1, D1, board);
        break;
      case C8:
        move_piece(A8, D8, board);
        break;
      case G1:
        move_piece(H1, F1, board);
        break;
      case G8:
        move_piece(H8, F8, board);
        break;
      default:
        ASSERT(false); // castling move had an illegal
                       // to square, so we just yeet ourselves out
    }
  }

  if(board->passant != NO_SQ) HASH_PAS;

  // remove the old castle state
  HASH_CAS;

  // store rest of the Undo struct fields
  board->history[board->hist_ply].move_played = move;
  board->history[board->hist_ply].move_counter = board->move_counter;
  board->history[board->hist_ply].passant = board->passant;
  board->history[board->hist_ply].castle_permission = board->castle_permission;

  // set the new castle permissions and hash them in
  board->castle_permission &= CASTLE_PERM[from];
  board->castle_permission &= CASTLE_PERM[to];
  board->passant = NO_SQ;
  HASH_CAS;

  /* 50 move rule and captures next */

  int32_t captured = CAPTURED(move);
  board->move_counter++;

  // reset the counter if a capture has occurred
  if(captured != EMPTY) {
    ASSERT(piece_valid(captured));
    clear_piece(to, board);
    board->move_counter = 0;
  }

  board->hist_ply++;
  board->ply++;

  if(PIECE_PAWN[board->pieces[from]]) {
    board->move_counter = 0;

    // pawn got pushed 2 squares to start
    if(move & MFLAGPS) {
      if(side == WHITE) {
        board->passant = from + 10;
        ASSERT(RANKS_BOARD[board->passant] == RANK_3);
      } else {
        board->passant = from - 10;
        ASSERT(RANKS_BOARD[board->passant] == RANK_6);
      }
      HASH_PAS;
    }
  }

  move_piece(from, to, board);

  // if we have a promotion, make sure it's a valid piece and the piece to
  // promote to isn't a pawn cause... can't promote a pawn to a pawn
  // then, clear the piece from the board and add the promoted piece
  int32_t promoted_piece = PROMOTED(move);
  if(promoted_piece != EMPTY) {
    ASSERT(piece_valid(promoted_piece) && !PIECE_PAWN[promoted_piece]);
    clear_piece(to, board);
    add_piece(to, board, promoted_piece);
  }

  if(PIECE_KING[board->pieces[to]]) {
    board->kings_sq[board->side] = to;
  }

  // flip flop the side
  board->side ^= 1;
  HASH_SIDE;

  check_board(board);

  // if after we move the king is in check, undo and it's a nope
  if(square_attacked(board->kings_sq[side], board->side, board)) {
    take_move(board);
    return false;
  }

  return true;
}

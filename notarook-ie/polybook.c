/**
 * This is the main file for handling the polyglot opening
 * book format.
 * See http://hgm.nubati.net/book_format.html for more
 */

#include "constants.h"
#include "functions.h"

// convert from our piece representation (enums.h) to the polyglot format
static const int32_t poly_kind_of_piece[13] = {-1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10};

/**
 * Initializes the given book.
 * Bookstr is set to the name of the book you can use (set in engine.c)
 */
bool init_polybook(Polybook_t *book, char *bookstr) {
  FILE *bk = fopen(bookstr, "rb");
  book->entries = NULL;

  if(!bk) {
    fprintf(stderr, "book file failed to be read!\n");
    exit(1);
  }

  // figure out file size
  fseek(bk, 0, SEEK_END);
  long end = ftell(bk);
  rewind(bk);

  // file is corrupt (too small to make sense)
  // this won't be a fatal error; we'll just go at it without the book
  if(end < (long)sizeof(PolybookEntry_t)) {
    fprintf(stderr, "book has no entries.\n");
    fclose(bk);
    return false;
  }

  book->num_entries = end / (long)sizeof(PolybookEntry_t);

  book->entries = calloc(book->num_entries, sizeof(PolybookEntry_t));

  // check if memory alloc succeeded
  // it'll prob never fail but it never hurts to be careful
  if(!book->entries) {
    fprintf(stderr, "memory alloc failed!\n");
    fclose(bk);
    exit(1);
  }

  // read in the entries
  fread(book->entries, sizeof(PolybookEntry_t), book->num_entries, bk);
  fclose(bk);
  return book->num_entries > 0;
}

/**
 * Resets a given book
 */
void clean_polybook(Polybook_t *book) {
  free(book->entries);
  book->num_entries = 0;
  book->entries = NULL;
}

/**
 * Check if an en passant pawn has enemy pawns adjacent to actually
 * make the capture
 */
static bool has_pawn_adjacent(const Board_t * board) {
  int32_t sq_w_pawn = 0;

  // what piece type are we looking for
  int32_t target_piece = (board->side == WHITE) ? wP : bP;

  if(board->passant != NO_SQ) {
    sq_w_pawn = board->passant + ((board->side == WHITE) ? (-10) : 10);

    if(board->pieces[sq_w_pawn + 1] == target_piece) return true;
    else if(board->pieces[sq_w_pawn - 1] == target_piece) return true;
  }
  return false;
}
/**
 * Turn the current board position into a polykey
 * so that we can look it up and use our opening book
 */
static uint64_t polykey_from_board(const Board_t *board) {

  int32_t sq = 0, rank = 0, file = 0;
  uint64_t ret = 0;

  int32_t piece = EMPTY;
  int32_t poly_piece = 0;
  int32_t offset = 0;

  // first, handle every piece
  for(sq = 0; sq < BOARD_SQ_NUM; ++sq) {
    piece = board->pieces[sq];
    if(piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
      ASSERT(piece >= wP && piece <= bK);

      // convert the piece from our rep to the polyboard book rep
      poly_piece = poly_kind_of_piece[piece];
      rank = RANKS_BOARD[sq];
      file = FILES_BOARD[sq];

      // this equation is from the site
      offset = (STANDARD_BOARD_SIZE * poly_piece) + (8 * rank) + file;
      ret ^= Random64[offset];
    }
  }

  // castling permissions next
  offset = 768; // again, from the website
  if(board->castle_permission & WKCAS) ret ^= Random64[offset + 0];
  if(board->castle_permission & WQCAS) ret ^= Random64[offset + 1];
  if(board->castle_permission & BKCAS) ret ^= Random64[offset + 2];
  if(board->castle_permission & BQCAS) ret ^= Random64[offset + 3];

  // in croissant
  // this format actually cares if a pawn is available for the capture, though
  offset = 772;
  if(has_pawn_adjacent(board)) {
    file = FILES_BOARD[board->passant];
    ret ^= Random64[offset + file];
  }

  // side
  if(board->side == WHITE) ret ^= Random64[780];
  return ret;
}

/**
 * Flip between book format and our format for the moves
 */
static uint32_t convert_poly_to_internal(uint16_t move, Board_t *board) {

  // this comes from the website
  int32_t ff = (move >> 6) & 7;
  int32_t fr = (move >> 9) & 7;
  int32_t tf = (move >> 0) & 7;
  int32_t tr = (move >> 3) & 7;
  int32_t pp = (move >> 12) & 7; // promotion

  char movestr[6];
  // no promotion piece
  if(pp == 0) {
    snprintf(movestr, sizeof(movestr), "%c%c%c%c",
      FILE_CHAR[ff], RANK_CHAR[fr], FILE_CHAR[tf], RANK_CHAR[tr]);
  } else {
    char prom = 'q';
    switch(pp) {
      case 1:
        prom = 'n';
        break;
      case 2:
        prom = 'b';
        break;
      case 3:
        prom = 'r';
        break;
    }
    snprintf(movestr, sizeof(movestr), "%c%c%c%c%c",
      FILE_CHAR[ff], RANK_CHAR[fr], FILE_CHAR[tf], RANK_CHAR[tr], prom);
  }

  // we already wrote a move parser, so go from their representation to ours via
  // our parser
  return parse_move(movestr, board);
}

/**
 * Called by the engine in order to find a book move for the position.
 * This isn't the most efficient way to do this, and maybe I'll come back
 * to this and will make it more efficient. Maybe.
 */
uint32_t get_book_move(Board_t *board, const Polybook_t book) {
  PolybookEntry_t *entry;
  int32_t count = 0;
  uint16_t move;
  uint64_t poly_key = polykey_from_board(board);
  uint32_t book_moves[MAX_BOOK_MOVES]; // stores the moves in the engine's format
  uint32_t tmp = NOMOVE;

  // loop through the book entries one by one
  // annoyingly, the book entries are big endian so we gotta flip em
  for(entry = book.entries; entry < book.entries + book.num_entries; ++entry) {
    if(poly_key == ntohll(entry->key)) {
      move = ntohs(entry->move);
      tmp = convert_poly_to_internal(move, board);

      if(tmp != NOMOVE) {
        book_moves[count++] = tmp;
        if(count > MAX_BOOK_MOVES) break;
      }
    }
  }

  // randomly select one of the book moves for this position
  // this is to ensure we don't play the same book opening every time
  return (count > 0) ? book_moves[rand() % count] : NOMOVE;

}

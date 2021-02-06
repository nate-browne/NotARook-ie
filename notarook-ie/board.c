/**
 * This file is for interacting with the board itself.
 * Unlike bboard.c, this isn't dealing with the bitboards directly.
 */

#include "functions.h"
#include "constants.h"

/**
 * Takes a FEN string and sets up the board accordingly
 * For more info on FEN, see https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
 */
bool parse_FEN(char *fen, Board_t *board) {
  ASSERT(fen);
  ASSERT(board);

  // reset our board and initialize
  int32_t rank = RANK_8, file = FILE_A;
  int32_t piece, count, ind, sq64, sq120;
  piece = count = ind = sq64 = sq120 = 0;

  reset_board(board);

  // stay in bounds and don't walk off the string
  while((rank >= RANK_1) && *fen) {
    count = 1;

    // the joys of non-functional programming...
    // granted, in haskell this would be a case block
    switch(*fen) {
      case 'p':
        piece = bP;
        break;
      case 'r':
        piece = bR;
        break;
      case 'n':
        piece = bN;
        break;
      case 'b':
        piece = bB;
        break;
      case 'k':
        piece = bK; // have it your way. dumb joke (i'm sorry)
        break;
      case 'q':
        piece = bQ;
        break;
      case 'P':
        piece = wP;
        break;
      case 'R':
        piece = wR;
        break;
      case 'N':
        piece = wN; // r/whiteknighting
        break;
      case 'B':
        piece = wB;
        break;
      case 'K':
        piece = wK;
        break;
      case 'Q':
        piece = wQ;
        break;

      // if not any of those, we've got a number
      // treat numbers (essentially) the same way
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
        piece = EMPTY;
        count = *fen - '0';
        break;

      // slashes and spaces are delimiters
      // we reset the file count and go down a rank on the board
      case '/':
      case ' ':
        --rank;
        file = FILE_A;
        ++fen;
        continue;

      default:
        fprintf(stderr, "FEN parsing error.\n");
        return false;
    }

    for( ; ind < count; ++ind) {
      sq64 = rank * 8 + file;
      sq120 = SQ120(sq64);

      if(piece != EMPTY) board->pieces[sq120] = piece;

      ++file;
    }

    // parsed this character, go to the next
    ++fen;
  }

  /* parse the rest of the FEN now since we've handled all of the characters */

  // at this point, the fen pointer should either be on a 'w' or a 'b'
  ASSERT(*fen == 'w' || *fen == 'b')

  // next move
  board->side = (*fen == 'w') ? WHITE : BLACK;
  fen += 2;

  // castling info
  for(ind = 0; ind < 4; ++ind) {
    if(*fen == ' ') break;
    
    // OR in each value depending on who can castle when and which side
    switch(*fen) {
      case 'K':
        board->castle_permission |= WKCAS;
        break;
      case 'Q':
        board->castle_permission |= WQCAS;
        break;
      case 'k':
        board->castle_permission |= BKCAS;
        break;
      case 'q':
        board->castle_permission |= BQCAS;
        break;
      default:
        break;
    }
    ++fen;
  }
  ++fen;

  // verify that castling was validly parsed
  ASSERT(board->castle_permission >= 0 && board->castle_permission <= 15);

  // en passant square
  if(*fen != '-') {

    // grab the coordinates of the square
    file = fen[0] - 'a';
    rank = fen[1] - '1';

    // verify that they make sense
    ASSERT(file >= FILE_A && file <= FILE_H);
    ASSERT(rank >= RANK_1 && rank <= RANK_8);

    if(board->side == WHITE) {
      ASSERT(rank == 6);
    } else {
      ASSERT(rank == 3);
    }

    board->passant = CONVERT_COORDS(file, rank);
    fen += 3;
  } else {
    fen += 2;
  }

  /* next section is handling the ply fields */
  char num_parsed[10];

  for(int32_t tmp = 0; tmp < 2; ++tmp) {
    memset(num_parsed, '\0', sizeof(num_parsed));

    ind = 0;

    while(*fen != ' ' && ind < 10) {
      num_parsed[ind] = *fen;
      ++fen;
      ++ind;
    }

    if(!tmp) {
      board->ply = atoi(num_parsed);
      ASSERT(isdigit(board->ply));
    } else {
      board->hist_ply = atoi(num_parsed);
      ASSERT(isdigit(board->hist_ply));
    }

    fen++;
  }

  // should be at the end of the string now
  ASSERT(*fen == '\0');


  board->hashkey = generate_hashkeys(board);

  return true;
}

void reset_board(Board_t *board) {
  int32_t ind = 0;

  // first, clear the board
  for( ; ind < BOARD_SQ_NUM; ++ind)
    board->pieces[ind] = OFFBOARD;

  for(ind = 0; ind < STANDARD_BOARD_SIZE; ++ind)
    board->pieces[SQ120(ind)] = EMPTY;

  for(ind = 0; ind < 3; ++ind) {
    board->big_pieces[ind] = 0;
    board->maj_pieces[ind] = 0;
    board->min_pieces[ind] = 0;
    board->pawns[ind] = (uint64_t)0;
  }

  for(ind = 0; ind < 13; ++ind)
    board->piece_num[ind] = 0;

  board->kings_sq[WHITE] = board->kings_sq[BLACK] = NO_SQ;

  board->side = BOTH;
  board->passant = NO_SQ;
  board->move_counter = 0;

  board->ply = board->hist_ply = 0;

  board->castle_permission = 0;

  board->hashkey = (uint64_t)0;

}

/**
 * This file is for interacting with the board itself.
 * Unlike bboard.c, this isn't dealing with the bitboards directly.
 */

#include "functions.h"
#include "constants.h"

/**
 * Checks if a given board position is actually valid.
 * Kind of a chonky boi of a function.
 * Returns void instead of a boolean cause it uses
 * our ASSERT macro to enforce validity, ending the program
 * if the board isn't valid.
 */
bool check_board(const Board_t * board) {
  int32_t temp_piece_num[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
  int32_t temp_big_piece[2] = {0,0};
  int32_t temp_maj_piece[2] = {0,0};
  int32_t temp_min_piece[2] = {0,0};
  int32_t temp_material[2] = {0,0};

  int32_t sq64, temp_piece, temp_pnum, sq120, color, piece_count;

  // used to validate pawn bit boards
  uint64_t temp_pawns[3] = {0, 0, 0};
  temp_pawns[WHITE] = board->pawns[WHITE];
  temp_pawns[BLACK] = board->pawns[BLACK];
  temp_pawns[BOTH] = board->pawns[BOTH];

  // first, validate the piece list
  for(temp_piece = wP; temp_piece <= bK; ++temp_piece) {
    for(temp_pnum = 0; temp_pnum < board->piece_num[temp_piece]; ++temp_pnum) {
      sq120 = board->piece_list[temp_piece][temp_pnum];
      ASSERT(board->pieces[sq120] == temp_piece);
    }
  }

  // next, validate piece count/other counts
  for(sq64 = 0; sq64 < STANDARD_BOARD_SIZE; ++sq64) {
    sq120 = SQ120(sq64);
    temp_piece = board->pieces[sq120];
    temp_piece_num[temp_piece]++;
    color = PIECE_COL[temp_piece];

    if(PIECE_BIG[temp_piece]) temp_big_piece[color]++;
    if(PIECE_MIN[temp_piece]) temp_min_piece[color]++;
    if(PIECE_MAJ[temp_piece]) temp_maj_piece[color]++;

    temp_material[color] += PIECE_VAL[temp_piece];
  }

  for(temp_piece = wP; temp_piece <= bK; ++temp_piece)
    ASSERT(temp_piece_num[temp_piece] == board->piece_num[temp_piece]);

  // validate the bitboard counts
  piece_count = count_bits(temp_pawns[WHITE]);
  ASSERT(piece_count == board->piece_num[wP]);

  piece_count = count_bits(temp_pawns[BLACK]);
  ASSERT(piece_count == board->piece_num[bP]);

  piece_count = count_bits(temp_pawns[BOTH]);
  ASSERT(piece_count == board->piece_num[wP] + board->piece_num[bP]);

  // validate the bitboard squares
  while(temp_pawns[WHITE]) {
    sq64 = pop_bit(&temp_pawns[WHITE]);
    ASSERT(board->pieces[SQ120(sq64)] == wP);
  }

  while(temp_pawns[BLACK]) {
    sq64 = pop_bit(&temp_pawns[BLACK]);
    ASSERT(board->pieces[SQ120(sq64)] == bP);
  }

  while(temp_pawns[BOTH]) {
    sq64 = pop_bit(&temp_pawns[BOTH]);
    ASSERT((board->pieces[SQ120(sq64)] == bP) || (board->pieces[SQ120(sq64)] == wP));
  }

  // check the material, piece counts, move, and the hashcode
  ASSERT(temp_material[WHITE] == board->material[WHITE] && temp_material[BLACK] == board->material[BLACK]);
  ASSERT(temp_min_piece[WHITE] == board->min_pieces[WHITE] && temp_min_piece[BLACK] == board->min_pieces[BLACK]);
  ASSERT(temp_maj_piece[WHITE] == board->maj_pieces[WHITE] && temp_maj_piece[BLACK] == board->maj_pieces[BLACK]);
  ASSERT(temp_big_piece[WHITE] == board->big_pieces[WHITE] && temp_big_piece[BLACK] == board->big_pieces[BLACK]);

  ASSERT(board->side == WHITE || board->side == BLACK);
  ASSERT(generate_hashkeys(board) == board->hashkey);

  // check the en passant value
  // either we don't have one, or if we do and it's white to move then it's on the 6th rank
  // or if it's black to move then it's on the 3rd rank
  ASSERT(board->passant == NO_SQ || (RANKS_BOARD[board->passant] == RANK_6 && board->side == WHITE)
          || (RANKS_BOARD[board->passant] == RANK_3 && board->side == BLACK));

  // make sure the kings are still on the board
  ASSERT(board->pieces[board->kings_sq[WHITE]] == wK);
  ASSERT(board->pieces[board->kings_sq[BLACK]] == bK);

  return true; // not really needed, but this allows this function to be placed in the ASSERT macro
}

/**
 * Mirrors the board so that we can check ourselves when evaluating
 */
void mirror_board(Board_t *board) {
  int32_t temp_pieces_array[STANDARD_BOARD_SIZE];
  int32_t temp_side = board->side ^ 1;

  int32_t swap_piece[13] = {EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK};
  int32_t temp_castle_perm = 0;
  int32_t temp_passant = NO_SQ;

  int32_t sq, tp;

  if(board->castle_permission & WKCAS) temp_castle_perm |= BKCAS;
  if(board->castle_permission & WQCAS) temp_castle_perm |= BQCAS;

  if(board->castle_permission & BKCAS) temp_castle_perm |= WKCAS;
  if(board->castle_permission & BQCAS) temp_castle_perm |= WQCAS;

  if(board->passant != NO_SQ) temp_passant = SQ120(MIRROR64(SQ64(board->passant)));

  for(sq = 0; sq < STANDARD_BOARD_SIZE; ++sq) {
    temp_pieces_array[sq] = board->pieces[SQ120(MIRROR64(sq))];
  }

  reset_board(board);

  for(sq = 0; sq < STANDARD_BOARD_SIZE; ++sq) {
    tp = swap_piece[temp_pieces_array[sq]];
    board->pieces[SQ120(sq)] = tp;
  }

  board->side = temp_side;
  board->castle_permission = temp_castle_perm;
  board->passant = temp_passant;

  board->hashkey = generate_hashkeys(board);

  update_material(board);

  ASSERT(check_board(board));
}

/**
 * Updates the values of the arrays for the material on the board
 */
void update_material(Board_t *board) {
  int32_t piece, sq, index, color;

  for(index = 0; index < BOARD_SQ_NUM; ++index) {
    sq = index;
    piece = board->pieces[index];

    if(piece != OFFBOARD && piece != EMPTY) {
      color = PIECE_COL[piece];

      // count up the pieces by type
      if(PIECE_BIG[piece]) board->big_pieces[color]++;
      if(PIECE_MAJ[piece]) board->maj_pieces[color]++;
      if(PIECE_MIN[piece]) board->min_pieces[color]++;

      // set up material values
      board->material[color] += PIECE_VAL[piece];

      // places pieces in the piece list
      board->piece_list[piece][board->piece_num[piece]++] = sq;

      // set up king square
      if(piece == wK || piece == bK) board->kings_sq[color] = sq;

      // handle pawns
      if(piece == wP) {
        SETBIT(board->pawns[WHITE], SQ64(sq));
        SETBIT(board->pawns[BOTH], SQ64(sq));
      } else if(piece == bP) {
        SETBIT(board->pawns[BLACK], SQ64(sq));
        SETBIT(board->pawns[BOTH], SQ64(sq));
      }
    }
  }
}

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

    for(ind = 0; ind < count; ++ind) {
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
  if(*fen != '-') {
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
  } else {
    ++fen;
  }
  ++fen;

  // verify that castling was validly parsed
  ASSERT(board->castle_permission >= 0 && board->castle_permission <= 0xF);

  // en passant square
  if(*fen != '-') {

    // grab the coordinates of the square
    file = fen[0] - 'a';
    rank = fen[1] - '1';

    // verify that they make sense
    ASSERT(file >= FILE_A && file <= FILE_H);
    ASSERT(rank >= RANK_1 && rank <= RANK_8);

    if(board->side == WHITE) {
      ASSERT(rank == RANK_6);
    } else {
      ASSERT(rank == RANK_3);
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

    while((*fen != ' ' && *fen != '\0') && ind < 10) {
      num_parsed[ind] = *fen;
      ++fen;
      ++ind;
    }

    if(!tmp) {
      board->ply = atoi(num_parsed);
    } else {
      board->hist_ply = atoi(num_parsed);
    }

    // if we're at the end of the string, don't advance more
    if(*fen) fen++;
  }

  board->hashkey = generate_hashkeys(board);

  // set the values of the board material up before returning
  update_material(board);
  return true;
}

/**
 * Reset and clear everything off of the board
 */
void reset_board(Board_t *board) {
  int32_t ind = 0;

  // first, clear the board
  for( ; ind < BOARD_SQ_NUM; ++ind)
    board->pieces[ind] = OFFBOARD;

  for(ind = 0; ind < STANDARD_BOARD_SIZE; ++ind)
    board->pieces[SQ120(ind)] = EMPTY;

  for(ind = 0; ind < 2; ++ind) {
    board->big_pieces[ind] = 0;
    board->maj_pieces[ind] = 0;
    board->min_pieces[ind] = 0;
    board->material[ind] = 0;
    board->pawns[ind] = (uint64_t)0;
  }

  // special case not handled by above loop
  board->pawns[BOTH] = (uint64_t)0;

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

/**
 * Print the current board struct state in a readable
 * to humans way.
 */
void print_board(const Board_t *board) {
  int32_t sq, file, rank, piece;


  printf("\nGame Board:\n\n");

  for(rank = RANK_8; rank >= RANK_1; --rank) {
    printf("%d  ", rank + 1);
    for(file = FILE_A; file <= FILE_H; ++file) {
      sq = CONVERT_COORDS(file, rank);
      piece = board->pieces[sq];
      printf("%3c", PIECE_CHAR[piece]);
    }
    printf("\n");
  }

  printf("\n   ");
  for(file = FILE_A; file <= FILE_H; ++file)
    printf("%3c", 'a' + file);

  printf("\n");

  printf("side to move: %c\n", SIDE_CHAR[board->side]);
  printf("turn: %d\n", board->hist_ply);
  printf("en passant square: %d\n", board->passant);
  printf("castling opportunities: %c %c %c %c\n",
          board->castle_permission & WKCAS ? 'K' : '-',
          board->castle_permission & WQCAS ? 'Q' : '-',
          board->castle_permission & BKCAS ? 'k' : '-',
          board->castle_permission & BQCAS ? 'q' : '-');
  printf("position hashcode: %llX\n", board->hashkey);
}

/**
 * File containing functions pertaining to move evaluation and selection
 */

#include "constants.h"
#include "functions.h"

static const int PAWN_TABLE[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

static const int KNIGHT_TABLE[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	5	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

static const int BISHOP_TABLE[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

static const int ROOK_TABLE[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

static const int MIRROR_64[64] = {
56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
};

/**
 * This function takes in a position on the board
 * and returns the evaluation of the position in
 * 100s of a pawn from the point of view of the side to move
 *
 * Pretty basic for now, but I'll improve on this eventually
 */
int32_t eval_position(const Board_t * board) {

  int32_t piece, square;

  // white == positive, black == negative
  // we negate at the end if black is to move
  int32_t score = board->material[WHITE] - board->material[BLACK];

  // white pawns first
  piece = wP;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score += PAWN_TABLE[SQ64(square)];
  }

  // black pawns next
  piece = bP;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score -= PAWN_TABLE[MIRROR64(SQ64(square))];
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
  }

  // black rooks
  piece = bR;
  for(int32_t idx = 0; idx < board->piece_num[piece]; ++idx) {
    square = board->piece_list[piece][idx];
    ASSERT(square_on_board(square));
    score -= ROOK_TABLE[MIRROR64(SQ64(square))];
  }

  return (board->side == WHITE) ? score : -score;
}

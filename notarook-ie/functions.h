/**
 * This short file is included in the main constants.h
 * as a way of separating out the extern function declarations,
 * keeping the other file (a little bit) shorter.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

/* init.c */
extern void init_all(void);
extern uint64_t RAND_64(void);

/* bboard.c */
extern void print_bboard(const uint64_t);
extern int32_t pop_bit(uint64_t *);
extern int32_t count_bits(uint64_t);

/* hashkey.c */
extern uint64_t generate_hashkeys(const Board_t *);

/* board.c */
extern void reset_board(Board_t *);
extern bool parse_FEN(char *, Board_t *);
extern void print_board(const Board_t *);
extern void update_material(Board_t *);
extern void check_board(const Board_t *);

/* attack.c */
extern bool square_attacked(const int32_t, const int32_t, const Board_t *);

/* printer.c */
extern char * print_square(const int32_t);
extern char * print_move(const uint32_t);
extern void print_movelist(const MoveList_t *);
extern uint32_t parse_move(char *, Board_t *);

/* movegen.c */
extern void generate_all_moves(const Board_t *, MoveList_t *);

/* validation.c */
extern bool square_on_board(const int32_t);
extern bool side_valid(const int32_t);
extern bool file_rank_valid(const int32_t);
extern bool piece_valid_empty(const int32_t);
extern bool piece_valid(const int32_t);

/* makemove.c */
extern bool make_move(Board_t *, uint32_t);
extern void take_move(Board_t *);
extern bool move_exists(Board_t *, const uint32_t);

/* perft.c */
extern uint64_t perft_test(int32_t, Board_t *, bool);

/* search.c */
extern void search_position(Board_t *, SearchInfo_t *);

/* util.c */
extern long get_time_millis(void);

/* hashset.c */
extern void init_hashset(PVTable_t *);
extern void store_move(Board_t *, const uint32_t);
extern uint32_t find_move(const Board_t *);
extern int32_t get_pv_line(const int32_t, Board_t *);

/* evaluate.c */
extern int32_t eval_position(const Board_t *);

#endif

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
extern void mirror_board(Board_t *);

/* attack.c */
extern bool square_attacked(const int32_t, const int32_t, const Board_t *);

/* printer.c */
extern char * print_square(const int32_t);
extern char * print_move(const uint32_t);
extern char * print_algebraic_move(const uint32_t, Board_t *);
extern uint32_t parse_move(char *, Board_t *);

/* movegen.c */
extern void generate_all_moves(const Board_t *, MoveList_t *);
extern void generate_all_captures(const Board_t *, MoveList_t *);
extern void init_MVV_LVA(void);

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
extern void make_null_move(Board_t *);
extern void take_null_move(Board_t *);

/* perft.c */
extern uint64_t perft_test(int32_t, Board_t *, bool);

/* search.c */
extern void search_position(Board_t *, SearchInfo_t *, bool, const Polybook_t);

/* util.c */
extern unsigned long get_time_millis(void);
extern void read_input(SearchInfo_t *);
extern uint16_t endian_swap_u16(uint16_t);
extern uint64_t endian_swap_u64(uint64_t);

/* hashset.c */
extern void init_hashset(PVTable_t *);
extern void clear_hashset(PVTable_t *);
extern void store_move(Board_t *, const uint32_t);
extern uint32_t find_move(const Board_t *);
extern int32_t get_pv_line(const int32_t, Board_t *);

/* evaluate.c */
extern int32_t eval_position(const Board_t *);

/* uci.c */
extern void UCI_loop(Board_t *, SearchInfo_t *, Polybook_t, bool);

/* xboard.c */
extern void XBoard_loop(Board_t *, SearchInfo_t *, Polybook_t, bool);
extern bool check_result(Board_t *);

/* console.c */
extern void console_loop(Board_t *, SearchInfo_t *, Polybook_t, bool);

/* polybook.c */
extern void clean_polybook(Polybook_t *);
extern bool init_polybook(Polybook_t *, char *);
extern uint32_t get_book_move(Board_t *, const Polybook_t);

#endif

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

#endif

/**
 * This short file is included in the main constants.h
 * as a way of separating out the extern function declarations,
 * keeping the other file (a little bit) shorter.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>

/* init.c */
extern void init_all(void);

/* bboard.c */
extern void print_bboard(const uint64_t);
extern int32_t pop_bit(uint64_t *);
extern int32_t count_bits(uint64_t);

#endif

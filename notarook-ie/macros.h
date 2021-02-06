/**
 * This file is to isolate macros created so that they don't disappear inside
 * of constants.h
 */

#ifndef MACROS_H
#define MACROS_H

#include <stdint.h>
#include <stdlib.h>

/* converts from file and rank to array number */
/* f is file, r is rank */
#define CONVERT_COORDS(f, r) ((21 + (f)) + ((r) * 10))

/* check if a bit is a 1 or a 0 */
#define CHECK_TAGGED(sq64, br) (((uint64_t)1 << (sq64)) & (br))

/* sets a bit to 0 */
#define CLRBIT(bb, sq) ((bb) &= CLEAR_MASK[(sq)])

/* sets a bit to 1 */
#define SETBIT(bb, sq) ((bb) |= SET_MASK[(sq)])

/* macros to make interacting with the indexed board arrays more straightforward */
#define SQ64(sq120) (ENGINE_TO_REGULAR[(sq120)])
#define SQ120(sq64) (REGULAR_TO_ENGINE[(sq64)])

#endif

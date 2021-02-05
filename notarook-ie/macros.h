/**
 * This file is to isolate macros created so that they don't disappear inside
 * of constants.h
 */

#ifndef MACROS_H
#define MACROS_H

/* converts from file and rank to array number */
/* f is file, r is rank */
#define CONVERT_COORDS(f,r) ((21 + f) + (r * 10))
#define CHECK_TAGGED(sq64, br) (((uint64_t)1 << sq64) & br)

#endif

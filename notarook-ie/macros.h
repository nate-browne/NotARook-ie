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

/* macros to shorten interactions with the arrays in consts.c */
#define IsBQ(p) (PIECE_BISHOP_QUEEN[(p)])
#define IsRQ(p) (PIECE_ROOK_QUEEN[(p)])
#define IsKn(p) (PIECE_KNIGHT[(p)])
#define IsKi(p) (PIECE_KING[(p)])

/* macros for interacting with the move struct */
#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m) >> 7) & 0x7F)
#define CAPTURED(m) (((m) >> 14) & 0xF)
#define PROMOTED(m) (((m) >> 20) & 0xF)

/* macros for generating moves and interacting with the board */
#define MOVE(f, t, ca, pro, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))
#define SQOFFBOARD(sq) (FILES_BOARD[sq] == OFFBOARD)

/* macros for hashing values into the position key */
#define HASH_PIECE(piece, sq) (board->hashkey ^= (PIECE_KEYS[(piece)][(sq)]))
#define HASH_CAS (board->hashkey ^= (CASTLE_KEYS[(board->castle_permission)]))
#define HASH_SIDE (board->hashkey ^= SIDE_KEY)
#define HASH_PAS (board->hashkey ^= (PIECE_KEYS[EMPTY][board->passant]))

/* macro for using the mirror array in evaluate.c */
#define MIRROR64(sq) (MIRROR_64[(sq)])

#endif

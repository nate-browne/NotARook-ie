/**
 * File defining all of the enums so that they're more easily found instead of
 * burying them in constants.h
 */

#ifndef ENUMS_H
#define ENUMS_H

/* define an enum for all of the pieces (including the null piece) */
/* w = white, b = black */
/* P = pawn, N = knight, B = bishop, Q = queen, K = king */
enum PIECES { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };

/* Next enums are for ranks and files */
/* Null file and null rank needed for the extra board padding */
enum FILES { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum RANKS { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

/* Next enum is for the colors */
enum COLORS { WHITE, BLACK, BOTH };

/* Next enum is for modes supported by the engine */
enum MODES { UCIMODE, XBOARDMODE, CONSOLEMODE };

/* next up are the board squares */
/* Note that OFFBOARD isn't strictly necessary (it's the same as NO_SQ) */
/* the purpose of it though is that it helps later with handling special moves */
enum BOARD_SQUARES {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

/* for castling purposes */
/* maps to the castle_permission 32 bit int in the board */
enum CASTLING { WKCAS = 1, WQCAS = 2, BKCAS = 4, BQCAS = 8 };

#endif

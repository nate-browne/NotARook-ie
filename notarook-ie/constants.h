#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include <stdbool.h>

#define NAME "NotARook-ie v1.0"

// number in half-moves
// this is a safe assumption; I don't think games can be 1024 full turns
#define MAX_GAME_MOVES 2048

#define BOARD_SQ_NUM 120
#define STANDARD_BOARD_SIZE 64

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

/* next up are the board squares */
enum BOARD_SQUARES {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ
};

/* for castling purposes */
/* maps to the castle_permission 32 bit int in the board */
enum CASTLING { WKCAS = 1, WQCAS = 2, BKCAS = 4, BQCAS = 8 };

// Struct used to store moves so that we can undo moves later (hence the name)
// for definitions of each member of the struct, see the board representation
// note: statuses are for before the move was played
typedef struct Undo {

  int32_t move_played;
  int32_t castle_permission;
  int32_t passant;
  uint8_t move_counter;
  uint64_t pos_key;

} Undo_t;

// our board representation
typedef struct Board {

  int32_t pieces[BOARD_SQ_NUM]; // the actual board itself, represented as a 1D array
  uint64_t pawns[3]; // bit array where if a bit is set to 1, a pawn is in that spot (one array index per color)

  int32_t kings_sq[2]; // tels what square the king is on

  bool side; // which side is supposed to move next (0 white, 1 black)
  int32_t passant; // set if an en passant square is available
  uint8_t move_counter; // check out the 50 rule move for chess
                        // for us, this is one hundo cause we count half moves
  int32_t ply; // number of half moves into the current search
  int32_t hist_ply; // in the total game, number of half moves made
                    // needed for figuring out repetitions

  int32_t castle_permission; // can we castle now?

  uint64_t pos_key; // unique key generated for each position generated

  int32_t piece_num[13]; // 13 instead of 12 cause of EMPTY at pos 0 in the PIECES enum
  int32_t big_pieces[3]; // anything that isn't a pawn
  int32_t maj_pieces[3]; // rooks and queens only
  int32_t min_pieces[3]; // bishops and knights only

  Undo_t history[MAX_GAME_MOVES];

} Board_t;

/* converts from file and rank to array number */
/* f is file, r is rank */
#define CONVERT_COORDS(f,r) ((21 + (f)) + ((r) * 10))


#endif

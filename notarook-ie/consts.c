/**
 * This file is for holding some constants used in the program.
 * The constants here are declared in "constants.h"
 */

#include "constants.h"

const char *PIECE_CHAR = ".PNBRQKpnbrqk";
const char *SIDE_CHAR = "wb-";
const char *RANK_CHAR = "12345678";
const char *FILE_CHAR = "abcdefgh";

bool PIECE_BIG[13] = {false, false, true, true, true, true, true, false, true, true, true, true, true};
bool PIECE_MAJ[13] = {false, false, false, false, true, true, true, false, false, false, true, true, true};
bool PIECE_MIN[13] = {false, false, true, true, false, false, false, false, true, true, false, false, false};
int32_t PIECE_VAL[13] = {0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000};
int32_t PIECE_COL[13] = {BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

const int KNIGHT_DIRS[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int ROOK_DIRS[4] = {-1, -10, 1, 10};
const int BISHOP_DIRS[4] = {-9, -11, 11, 9};
const int KING_DIRS[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

bool PIECE_PAWN[13] = {false, true, false, false, false, false, false, true, false, false, false, false, false};
bool PIECE_KING[13] = {false, false, false, false, false, false, true, false, false, false, false, false, true};
bool PIECE_ROOK_QUEEN[13] = {false, false, false, false, true, true, false, false, false, false, true, true, false};
bool PIECE_BISHOP_QUEEN[13] = {false, false, false, true, false, true, false, false, false, true, false, true, false};
bool PIECE_SLIDES[13] = {false, false, false, true, true, true, false, false, false, true, true, true, false};
bool PIECE_KNIGHT[13] = {false, false, true, false, false, false, false, false, true, false, false, false, false};

const int32_t MIRROR_64[STANDARD_BOARD_SIZE] = {
56 , 57 , 58 , 59 , 60 , 61 , 62 , 63 ,
48 , 49 , 50 , 51 , 52 , 53 , 54 , 55 ,
40 , 41 , 42 , 43 , 44 , 45 , 46 , 47 ,
32 , 33 , 34 , 35 , 36 , 37 , 38 , 39 ,
24 , 25 , 26 , 27 , 28 , 29 , 30 , 31 ,
16 , 17 , 18 , 19 , 20 , 21 , 22 , 23 ,
8 , 9 , 10 , 11 , 12 , 13 , 14 , 15 ,
0 , 1 , 2 , 3 , 4 , 5 , 6 , 7
};

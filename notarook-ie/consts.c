/**
 * This file is for holding some constants used in the program.
 * The constants here are declared in "constants.h"
 */

#include "constants.h"

const char PIECE_CHAR[] = ".PNBRQKpnbrqk";
const char SIDE_CHAR[] = "wb-";
const char RANK_CHAR[] = "12345678";
const char FILE_CHAR[] = "abcdefgh";

bool PIECE_BIG[13] = {false, false, true, true, true, true, true, false, true, true, true, true, true};
bool PIECE_MAJ[13] = {false, false, false, false, true, true, true, false, false, false, true, true, true};
bool PIECE_MIN[13] = {false, false, true, true, false, false, false, false, true, true, false, false, false};
int32_t PIECE_VAL[13] = {0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000};
int32_t PIECE_COL[13] = {BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

const int KNIGHT_DIRS[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int ROOK_DIRS[4] = {-1, -10, 1, 10};
const int BISHOP_DIRS[4] = {-9, -11, 11, 9};
const int KING_DIRS[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

bool PIECE_KNIGHT[13] = {false, false, true, false, false, false, false, false, true, false, false, false, false};
bool PIECE_KING[13] = {false, false, false, false, false, false, true, false, false, false, false, false, true};
bool PIECE_ROOK_QUEEN[13] = {false, false, false, false, true, true, false, false, false, false, true, true, false};
bool PIECE_BISHOP_QUEEN[13] = {false, false, false, true, false, true, false, false, false, true, false, true, false};

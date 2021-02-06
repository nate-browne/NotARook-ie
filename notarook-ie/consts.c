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

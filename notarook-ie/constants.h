/**
 * This main header file contains the other included headers, as
 * well as many of the definitions used throughout the other files
 * in this program.
 * 
 * Yeah, it's messy. Maybe at some point I'll actually separate things
 * out more granularly into other header files, but for now this will do
 * just fine, thank you.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "functions.h"
#include "enums.h"
#include "macros.h"

/*
 * Hello valued reader of this source code!
 *
 * Throughout reading, you'll see a lot of instances of
 * uint64_t values. The reason for this is that I want
 * to guarantee that I will have fixed-width 64 bit unsigned
 * values to use a bitboards to track pieces throughout the program.
 * The reason I didn't use an "unsigned long long" is to guarantee
 * that no matter what OS runs this code, the size will be the same.
 */

#define DEBUG

// kind of messy, but this whole mess allows us to throw in
// assert debug statements without mass commenting them out later
// just comment out line 10
// to be clear, I took this from a book and didn't come up with this macro
// myself
#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed!!", #n); \
printf(" on %s ", __DATE__); \
printf("at %s ", __TIME__); \
printf("in file %s ", __FILE__); \
printf("at line %d ", __LINE__); \
exit(1);}
#endif

#define NAME "NotARook-ie v1.0.0"

// number in half-moves
// this is a safe assumption; I don't think games can be 1024 full turns
#define MAX_GAME_MOVES 2048

#define BOARD_SQ_NUM 120
#define STANDARD_BOARD_SIZE 64

#define MAGIC_BIT_NUM 0x783A9B23

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

  // 13 instead of 12 cause of EMPTY at pos 0 in the PIECES enum
  // used to speed up move gen and the search stage
  int32_t piece_list[13][10];

} Board_t;


// goes from [0, 120) to [0, 64)
// in other words, convert an engine chess board to the regular
extern int32_t ENGINE_TO_REGULAR[BOARD_SQ_NUM];

// goes from [0, 64) to [0, 120)
// in other words, convert a regular chess board to the engine
extern int32_t REGULAR_TO_ENGINE[STANDARD_BOARD_SIZE];

#endif

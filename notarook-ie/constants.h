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
#include <ctype.h>
#include <string.h>
#include <stdio.h>

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
// just comment out line 35
// to be clear, I took this from a book and didn't come up with this macro
// myself
#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - \033[91mFAILED!!\033[0m", #n); \
printf(" on %s ", __DATE__); \
printf("at %s ", __TIME__); \
printf("in file %s ", __FILE__); \
printf("on line %d\n", __LINE__); \
exit(1);}
#endif

#define NAME "NotARook-ie v1.0.0"

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define TRICKY_FEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

// number in half-moves
// this is a safe assumption; I don't think games can be 1024 full turns
#define MAX_GAME_MOVES 2048

// max number of moves that we expect there to be in any given position
#define MAX_POSITION_MOVES 256

#define BOARD_SQ_NUM 120
#define STANDARD_BOARD_SIZE 64

// max search depth to use
#define MAX_DEPTH 64

// why this value in particular? no freaking idea.
// it just worked so... ¯\_(ツ)_/¯
// you can find more about it here: https://www.chessprogramming.org/Looking_for_Magics
#define MAGIC_BIT_NUM 0x783A9B23

// initial size of our hashset
// should be ~2MB
#define HASHSET_SIZE 0x10000000000

// Struct used to store moves so that we can undo moves later (hence the name)
// For definitions of each member of the struct, see the board representation
// Note: statuses are for before the move was played
typedef struct Undo {

  uint32_t move_played;
  int32_t castle_permission;
  int32_t passant;
  int32_t move_counter;
  uint64_t hashkey;

} Undo_t;

/**
 *                        MOVE REPRESENTATION
 * this engine will use 32 bit unsigned integers to represent moves.
 * We could use signed, but since we're using only the least significant 7 nibbles,
 * it doesn't really matter. Plus, it's generally safer to use unsigned when manipulating
 * bits.
 * 
 * The layout is as follows
 * 0000 0000 0000 0000 0000 0111 1111 -> From section of the move (0x7F)
 * 0000 0000 0000 0011 1111 1000 0000 -> To section of the move (0x7F shifted << by 7)
 * 0000 0000 0011 1100 0000 0000 0000 -> captured piece (if any) (0xF shifted << by 14)
 * 0000 0000 0100 0000 0000 0000 0000 -> en passant (a.k.a. on croissant) (0x40000)
 * 0000 0000 1000 0000 0000 0000 0000 -> pawn start (0x80000 or 0x8 shifted by 16)
 * 0000 1111 0000 0000 0000 0000 0000 -> promotion (0xF00000 or 0xF shifted by 20) 
 * 0001 0000 0000 0000 0000 0000 0000 -> castling (0x1000000)
 * 
 * To see the macros that we use to interact with these, check macros.h
 */

// not macros, but useful flags for the above idea
#define MFLAGEP 0x40000 // en passant
#define MFLAGPS 0x80000 // pawn start
#define MFLAGCAS 0x1000000 // castle
#define MFLAGCAP 0x7C000 // capture (of any kind)
#define MFLAGPR 0xF00000 // promotion

// for alpha beta evaluation
#define NOMOVE 0


// Struct for representing a move
typedef struct Move {
  uint32_t move; // the move played
  int32_t score; // the score associated with that move
} Move_t;

// struct for holding a list of the above move struct
typedef struct MoveList {

  Move_t moves[MAX_POSITION_MOVES];
  int32_t count;
} MoveList_t;

// Entries to our hashset are of this type
// hold the hashkey for that position as well as the move
// These are entries that are better than alpha when alpha-beta searching
typedef struct PVEntry {
  uint64_t hashkey;
  uint32_t move;
} PVEntry_t;

// Definition of our hashset
// Contains an array of entries as well as how many entries we have
typedef struct PVTable {
  PVEntry_t *table;
  unsigned long entries;
} PVTable_t;

// struct for holding relevant search tree info
typedef struct SearchInfo {
  long starttime;
  long stoptime;
  int32_t depth;
  int32_t max_depth;
  int32_t max_time;
  int32_t moves_to_go;

  long nodes; // count of nodes visited

  bool quit; // this is to end the program
  bool stopped; // this is to stop the search
  bool infinite; // no time control, search as much as possible
} SearchInfo_t;

// our board representation
typedef struct Board {

  int32_t pieces[BOARD_SQ_NUM]; // the actual board itself, represented as a 1D array
  uint64_t pawns[3]; // bit array where if a bit is set to 1, a pawn is in that spot (one array index per color)

  int32_t kings_sq[2]; // tels what square the king is on

  bool side; // which side is supposed to move next (0 white, 1 black)
  int32_t passant; // set if an en passant square is available
  int32_t move_counter; // check out the 50 rule move for chess
                        // for us, this is one hundo cause we count half moves
  int32_t ply; // number of half moves into the current search
  int32_t hist_ply; // in the total game, number of half moves made
                    // needed for figuring out repetitions

  int32_t castle_permission; // can we castle now?

  uint64_t hashkey; // unique key generated for each position generated

  int32_t piece_num[13]; // 13 instead of 12 cause of EMPTY at pos 0 in the PIECES enum
  int32_t big_pieces[2]; // anything that isn't a pawn
  int32_t maj_pieces[2]; // rooks and queens only
  int32_t min_pieces[2]; // bishops and knights only
  int32_t material[2]; // holds value of material for black and white

  Undo_t history[MAX_GAME_MOVES];

  // 13 instead of 12 cause of EMPTY at pos 0 in the PIECES enum
  // used to speed up move gen and the search stage
  int32_t piece_list[13][10];

  // instance of our hashset
  PVTable_t pvt;

  // array of moves stored for a best line
  uint32_t pv_array[MAX_DEPTH];

  // both of the following arrays are for storing non-captures

  // increment the piece types number if a move beats alpha
  uint32_t search_history[13][BOARD_SQ_NUM];

  // two moves that most recently caused a beta cutoff
  uint32_t search_killers[2][MAX_DEPTH];

} Board_t;


// goes from [0, 120) to [0, 64)
// in other words, convert an engine chess board to the regular
extern int32_t ENGINE_TO_REGULAR[BOARD_SQ_NUM];

// goes from [0, 64) to [0, 120)
// in other words, convert a regular chess board to the engine
extern int32_t REGULAR_TO_ENGINE[STANDARD_BOARD_SIZE];

// masks for setting and clearing bitboards
extern uint64_t SET_MASK[STANDARD_BOARD_SIZE];
extern uint64_t CLEAR_MASK[STANDARD_BOARD_SIZE];

// need to index by piece as well as by the square
extern uint64_t PIECE_KEYS[13][BOARD_SQ_NUM];

// only hashing if it's white's turn, so just need one
extern uint64_t SIDE_KEY;
extern uint64_t CASTLE_KEYS[16];

// used when printing the board in board.c
extern const char *PIECE_CHAR;
extern const char *SIDE_CHAR;
extern const char *RANK_CHAR;
extern const char *FILE_CHAR;

// used to answer question "what rank/file is <square> on?"
extern int32_t FILES_BOARD[BOARD_SQ_NUM];
extern int32_t RANKS_BOARD[BOARD_SQ_NUM];

// used to ask a true false question for each piece
extern bool PIECE_BIG[13];
extern bool PIECE_MAJ[13];
extern bool PIECE_MIN[13];
extern bool PIECE_KNIGHT[13];
extern bool PIECE_KING[13];
extern bool PIECE_ROOK_QUEEN[13];
extern bool PIECE_BISHOP_QUEEN[13];
extern bool PIECE_SLIDES[13];
extern bool PIECE_PAWN[13];

// used for keeping track of values and colors of board pieces
extern int32_t PIECE_VAL[13];
extern int32_t PIECE_COL[13];

// used to get directions pieces can move
extern const int KNIGHT_DIRS[8];
extern const int ROOK_DIRS[4];
extern const int BISHOP_DIRS[4];
extern const int KING_DIRS[8];

#endif

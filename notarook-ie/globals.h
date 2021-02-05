#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

#include "constants.h"

// goes from [0, 120) to [0, 64)
// in other words, convert an engine chess board to the regular
extern int32_t ENGINE_TO_REGULAR[BOARD_SQ_NUM];

// goes from [0, 64) to [0, 120)
// in other words, convert a regular chess board to the engine
extern int32_t REGULAR_TO_ENGINE[STANDARD_BOARD_SIZE];

extern void init_all(void);
extern void init_120_to_64(void);

#endif

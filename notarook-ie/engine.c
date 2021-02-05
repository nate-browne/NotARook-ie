#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "globals.h"

int main(int argc, char * argv[]) {
  init_all();

  int32_t index;

  for(index = 0; index < BOARD_SQ_NUM; ++index) {
    if(!(index % 10)) printf("\n");
    printf("%5d", ENGINE_TO_REGULAR[index]);
  }

  printf("\n");
  printf("\n");

  for(index = 0; index < STANDARD_BOARD_SIZE; ++index) {
    if(!(index % 8)) printf("\n");
    printf("%5d", REGULAR_TO_ENGINE[index]);
  }

  printf("\n");

  return EXIT_SUCCESS;
}

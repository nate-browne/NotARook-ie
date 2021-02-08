/**
 * This function runs the entire suite of perft tests in the provided
 * .epd file. This would have been a ton easier in python, but oh whale.
 */

#include "constants.h"
#include "functions.h"

#define EXPECTED_ARG_COUNT 3

static void die(const char * reason) {
  fprintf(stderr, "%s\n", reason);
  exit(1);
}

static uint64_t grab_answer_value(char *rest, int32_t depth) {
  uint64_t res = 0;

  while(rest) {
    char val = rest[1];
    int32_t num_val = val - '0';

    if(num_val == depth) {
      res = strtoll(rest + 3, NULL, 10);
      break;
    }
    rest = strtok(NULL, ";");
  }

  return res;
}

int main(int argc, char *argv[]) {

  if(argc != EXPECTED_ARG_COUNT) die("Expected two arguments.\nUSAGE: ./stresstest <depth int [1, 6]> <filename>.epd\n");

  init_all();

  // first up, parse the depth
  int32_t depth = atoi(argv[1]);
  if(!depth) die("Expected integer n with 0 < n < 7 for depth\n");

  // next up, parse the filename
  char *filename = argv[2];

  char instr[BUFSIZ], *token, fen[BUFSIZ];
  memset(fen, '\0', BUFSIZ);

  FILE *file = fopen(filename, "r");
  if(!file) perror("Failed to open the file.");

  printf("*************************STARTING PERFT TEST*************************\n");
  printf("Chosen depth: %d\n", depth);
  printf("\n\033[93mWARNING:\033[0m Depending on the depth chosen, this may take a while.\n");
  printf("Press return/enter to continue...\n");
  (void)getchar();

  int count = 1;

  // loop through the file line by line
  while(fgets(instr, BUFSIZ, file)) {

    Board_t board;
    token = strtok(instr, ";");
    strncpy(fen, token, BUFSIZ);

    parse_FEN(token, &board);

    token = strtok(NULL, ";");

    // grab the answer for what number we expect given the depth
    uint64_t expected = grab_answer_value(token, depth);
    uint64_t result = perft_test(depth, &board, true);

    if(expected == result) {
      printf("Test %d (FEN: %s): \033[92mPASSED\033[0m (expected: %lld got: %lld)\n", count++, fen, expected, result);
    } else {
      printf("Test %d (FEN: %s): \033[91mFAILED\033[0m (expected: %lld got: %lld)\n", count++, fen, expected, result);
      exit(1);
    }
  }

  printf("\nALL TESTS PASSED SUCCESSFULLY\n");
  printf("*************************ENDING PERFT TEST*************************\n");

  fclose(file);
  return EXIT_SUCCESS;
}

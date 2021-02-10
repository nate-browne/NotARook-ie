/**
 * This function runs the entire suite of perft tests in the provided
 * .epd file. This would have been a ton easier in python, but oh whale.
 *
 * If you're gonna use your own .epd file, make sure it's formatted like the one
 * provided else this script won't work.
 */

#include "constants.h"
#include "functions.h"

#define EXPECTED_ARG_COUNT 3

static void die(const char * reason) {
  fprintf(stderr, "\n\t\033[91mERROR\033[0m: ");
  fprintf(stderr, "%s\n\n", reason);
  exit(1);
}

static uint64_t grab_answer_value(char *rest, int32_t depth) {

  while(rest) {
    char val = rest[1];
    int32_t num_val = val - '0';

    if(num_val == depth) {
      return strtoull(rest + 3, NULL, 10);
    }
    rest = strtok(NULL, ";");
  }

  return 0;
}

int main(int argc, char *argv[]) {

  if(argc != EXPECTED_ARG_COUNT) {
    char die_str[BUFSIZ];
    snprintf(die_str, BUFSIZ, "Expected two arguments.\n\n\tUSAGE: %s <depth int [1, 6]> <filename>.epd", argv[0]);
    die(die_str);
  }

  init_all();

  // first up, parse the depth
  int32_t depth = atoi(argv[1]);
  if(!depth) {
    char die_str[BUFSIZ];
    snprintf(die_str, BUFSIZ, "Depth \"%s\" isn't valid. Make sure your depth is within the bounds in your .epd file.", argv[1]);
    die(die_str);
  }

  // next up, parse the filename
  char *filename = argv[2];

  char instr[BUFSIZ], *token, fen[BUFSIZ];
  memset(fen, '\0', BUFSIZ);

  FILE *file = fopen(filename, "r");
  if(!file) {
    char die_str[BUFSIZ];
    snprintf(die_str, BUFSIZ, "Failed to open file \"%s\" (are you sure it exists/is spelt correctly?)", argv[2]);
    die(die_str);
  }

  printf("*************************STARTING PERFT TEST*************************\n");
  printf("Chosen depth: %d\n", depth);
  printf("\n\033[93mWARNING:\033[0m Depending on the depth chosen, this may take a while.\n");
  printf("Press return/enter to continue...\n");
  (void)getchar();

  int count = 1;
  bool failed = false;

  // loop through the file line by line
  while(fgets(instr, BUFSIZ, file)) {

    Board_t board;
    token = strtok(instr, ";");
    strncpy(fen, token, BUFSIZ);

    parse_FEN(token, &board);

    token = strtok(NULL, ";");

    // grab the answer for what number we expect given the depth
    uint64_t expected = grab_answer_value(token, depth);

    if(!expected) {
      char die_str[BUFSIZ];
      snprintf(die_str, BUFSIZ, "Depth %d is outside of the bounds of your perft test file.", depth);
      die(die_str);
    }

    uint64_t result = perft_test(depth, &board, true);

    printf("Test %d (FEN: %s): ", count++, fen);
    if(expected == result) {
      printf("\033[92mPASSED\033[0m (expected: %lld got: %lld)\n", expected, result);
    } else {
      printf("\033[91mFAILED\033[0m (expected: %lld got: %lld)\n", expected, result);
      failed = true;
    }
  }

  if(!failed) printf("\n                  ALL TESTS PASSED SUCCESSFULLY\n");
  else printf("\nOne or more tests failed.\n");

  printf("*************************ENDING PERFT TEST*************************\n");

  fclose(file);
  return EXIT_SUCCESS;
}

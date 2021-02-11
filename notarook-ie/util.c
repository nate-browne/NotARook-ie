/**
 * Contains small utility functions for use in other places.
 */


#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/select.h>
#endif

#include "functions.h"
#include "constants.h"

/**
 * Returns the current time solely in milliseconds
 */
int32_t get_time_millis(void) {

#ifdef WIN32

  return GetTickCount();

#else

  struct timeval tm;

  gettimeofday(&tm, NULL);

  // struct returns seconds and microseconds, so convert accordingly
  return (tm.tv_sec * 1000) + (tm.tv_usec / 1000);

#endif
}

/**
 * Peeks stdin to see if we need to interrupt our engine while it is thinking
 */
static bool input_waiting() {

#ifndef WIN32

  fd_set readfds;
  struct timeval tv;
  FD_ZERO(&readfds);
  FD_SET(fileno(stdin), &readfds);

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  select(16, &readfds, 0, 0, &tv);

  return FD_ISSET(fileno(stdin), &readfds);

#else

  static int init = 0, pipe;
  static HANDLE inh;
  DWORD dw;

  if(!init) {
    init = 1;
    inh = GetStdHandle(STD_INPUT_HANDLE);
    pipe = !GetConsoleMode(inh, &dw);
    if(!pipe) {
      SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
      FlushConsoleInputBuffer(inh);
    }
  }
  if(pipe) {
    if(!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
    return dw;
  } else {
    GetNumberOfConsoleInputEvents(inh, &dw);
    return dw <= 1 ? 0 : dw;
  }

#endif
}

/**
 * Reads in standard input values if there is indeed
 * input to be read
 */
void read_input(SearchInfo_t *info) {
  int32_t bytes;
  char input[256] = "", *endc;

  if(input_waiting()) {
    info->stopped = true;
    do {
      bytes = read(fileno(stdin), input, 256);
    } while(bytes < 0);
    endc = strchr(input, '\n');
    if(endc) *endc = '\0';

    if(strlen(input) > 0) {
      if(!strncmp(input, "quit", strlen("quit"))) {
        info->quit = true;
      }
    }
  }
}

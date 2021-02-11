/**
 * Contains small utility functions for use in other places.
 */

#include <sys/time.h>
#include "functions.h"
#include "constants.h"

/**
 * Returns the current time solely in milliseconds
 */
long get_time_millis(void) {
  struct timeval tm;

  gettimeofday(&tm, NULL);

  // struct returns seconds and microseconds, so convert accordingly
  return (tm.tv_sec * 1000) + (tm.tv_usec / 1000);
}

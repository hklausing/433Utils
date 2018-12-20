/*
  File      helper.cpp

  Some helper function for this project.
*/

#include "helper.h"

/*
 *
 */
int ms_sleep(unsigned long msec)
{
  int result = 0;

  {
    struct timespec ts_remaining = {
			(__time_t)(msec / 1000), 						// seconds
      (__syscall_slong_t)((msec % 1000) * 1000000L) 	// nano seconds
    };

    do {

      struct timespec ts_sleep = ts_remaining;
      result = nanosleep(&ts_sleep, &ts_remaining);

    } while ((EINTR == errno) && (-1 == result));
  }

  if (-1 == result) {
    perror("nanosleep() failed");
  }

  return result;

}


/*
 *
 */
const char* getTimeStamp() {

	#define MAX_SIZE  100

	static char buf[MAX_SIZE];
	timeval tp;
	gettimeofday(&tp, 0);
	time_t curtime = tp.tv_sec;
	tm *t = localtime(&curtime);
	snprintf(buf, MAX_SIZE, "%4d-%02d-%02dT%02d:%02d:%02d:%03d"
			, (1900+t->tm_year), (t->tm_mon+1), t->tm_mday
			, t->tm_hour, t->tm_min, t->tm_sec, tp.tv_usec/1000);

	return buf;
}

// vim: autoindent tabstop=2 shiftwidth=2 expandtab softtabstop=2

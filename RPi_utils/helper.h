/*
  File      helper.h

  Some helper function for this project.
*/

#ifndef HELPER_H
#define HELPER_H


#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>


/*
 * ms_sleep
 *
 * Wait the defined amount of milli seconds
 *
 * Param1:  value of milli seconds.
 */
int ms_sleep(unsigned long msec);


/*
 * getTimeStamp
 *
 * Returns a string of long ISO8601 time stamp with number of milli seconds
 *
 * Param1:  -
 * Return:  String with ISO8601 long time information
 */
const char* getTimeStamp(void);


#endif    // #ifndef HELPER_H

// vim: autoindent tabstop=2 shiftwidth=2 expandtab softtabstop=2

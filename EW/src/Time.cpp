// wdkg 2008-2010

#ifdef _WIN32
#include <time.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include "ew/Time.h"

ew::Time::Time() :
 first_elapsed(true)
// start_time uninitialized
{
#ifdef _WIN32
  GetSystemTimeAsFileTime(&start_time);
#else
  gettimeofday(&start_time, 0);
#endif
}

double
ew::Time::real_elapsed()
{
// This is to ensure initial time is exactly 0.0.
  if (first_elapsed) {
    first_elapsed = false;
    return 0.0;
  }
  double t;
// Careful here:
//   don't rely on size and signedness of the components
//   avoid intermediate overflow or loss of precision
// Ignore date roll over issues and overflow in final result.
// This did not work
//   t = ((tv.dwHighDateTime - start_time.dwHighDateTime) * 4294967296.0 +
//   tv.dwLowDateTime - start_time.dwLowDateTime) * 1.0e-7;
#ifdef _WIN32
  FILETIME tv;
  GetSystemTimeAsFileTime(&tv);
  t = (tv.dwHighDateTime - start_time.dwHighDateTime) * 429.4967296 +
   tv.dwLowDateTime * 1.0e-7 - start_time.dwLowDateTime * 1.0e-7;
#else
  struct timeval tv;
  gettimeofday(&tv, 0);
  t = (tv.tv_sec - start_time.tv_sec) * 1.0 +
   (tv.tv_usec * 1.0e-6 - start_time.tv_usec * 1.0e-6);
#endif
  return t;
}

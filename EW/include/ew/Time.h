#ifndef EW_TIME_H_INCLUDED
#define EW_TIME_H_INCLUDED

// wdkg 2008-2010

// This header file is an internal part of the library.
// The class is not part of the library interface.
// This is a utility, included with the public headers purely as a convenience.

// Time utility

// With gcc ew::Time::elapsed() is thread safe, unless safe local static
// initialization has been disabled.
// With mingw it is thread safe with the -mthreads compile and link option.
// It is safe anyway if first called while the program is single threaded.
// Otherwise it is not necessarily thread safe, and its behaviour is undefined.

// The first call to elapsed() will return exactly 0.0.

#ifdef _WIN32
#include <time.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif

namespace ew {

  class Time {
  public:
#ifdef __GNUC__
    inline static double elapsed() __attribute((nothrow));
#else
    inline static double elapsed();
#endif
  private:
    Time();
    double real_elapsed();
    bool first_elapsed;
#ifdef _WIN32
    FILETIME start_time;
#else
    struct timeval start_time;
#endif
  };

}

inline double
ew::Time::elapsed()
{
// static variable
  static ew::Time StaticTime;
  return StaticTime.real_elapsed();
}

#endif

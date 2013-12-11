#ifndef EW_DEBUG_H_INCLUDED
#define EW_DEBUG_H_INCLUDED

// wdkg 2008-2010

// This header file is an internal part of the library.
// The class is not part of the library interface.
// This is a utility, included with the public headers purely as a convenience.

// Debug Message facility

// This is activated by environmental variables, which determine the
// destination of the messages (file or stdout), and which messages to output.

// The constructor can throw bad_alloc and runtime_error.
// All methods will catch exceptions and convert them to warnings in the
// debugging stream.
// A missing termination to the debugging stream indicates an exception that
// couldn't be reported.

// An interface to change the destination or pattern dynamically would require
// a more complicated implementation, but would be useful in scriptable
// applications.

// This is intended to have minimal overhead when debugging is not enabled.
// Debugging speed is not a priority.

// ew::Debug::dprintf returns bool (always true) so that the method can be used
// after &&.

// A message beginning with "!" is considered an alert.
// A warning will be issued on closing if any alerts have been output.

// With gcc, ew::Debug::Instance() is thread safe, unless safe local static
// initialization has been disabled.
// With mingw it is thread safe with the -mthreads compile and link option.
// It is safe, irrespective, if first called while the program is single
// threaded.

// Outside of the constructor, which is only called once, all members are
// are accessed in a thread safe way.

#include <cstdarg>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace ew {

  class Debug {
  public:
    inline static const ew::Debug& instance();
    inline bool get_active(const char *classname) const;
#ifdef __GNUC__
    bool dprintf(const char *msg, ...) const
     __attribute((format(printf,2,3),nothrow));
#else
    bool dprintf(const char *msg, ...) const;
#endif
    bool dvprintf(const char *msg, va_list ap) const;
    static const char *to_str(bool v);
  private:
    Debug();
    ~Debug();
#ifdef __GNUC__
    bool get_active_helper(const char *classname) const __attribute((nothrow));
#else
    bool get_active_helper(const char *classname) const;
#endif
    bool active;
    bool is_file;
    mutable bool alerts;
    mutable bool failures;
    const char *pattern;
    std::ofstream file_str;
    std::ostream *str;
// undefined
    Debug(const ew::Debug &that);
    Debug &operator=(const ew::Debug &that);
  };

}

inline const ew::Debug&
ew::Debug::instance()
{
// static variable
  static ew::Debug StaticDbg;
  return StaticDbg;
}

inline bool
ew::Debug::get_active(const char *classname) const
{
  if (!active) {
    return false;
  }
  return get_active_helper(classname);
}

#endif

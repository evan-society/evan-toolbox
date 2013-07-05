#ifndef EW_STRING_H_INCLUDED
#define EW_STRING_H_INCLUDED

// wdkg 2008-2010

// This header file is an internal part of the library.
// The class is not part of the library interface.
// This is a utility, included with the public headers purely as a convenience.

// String utilities:
//   scan and format in POSIX/C locale, irrespective of the current locale
//   simple pattern matching
//   sprintf with memory management

// No exceptions are thrown in scan and format.
//XXX all terminate by '\n' so can efficiently handle mmap files terminated by
// '\n'

// Does not write float with a final ".0".
// Faithfully write/read except does not reproduce data load of floating point
// nan's.
// Will read "nan(xxx)" float form, but only if closing parenthisis exists
//XXX  and no whitespace in contents
// Reads case insensitive [+-]nan, [+-]inf and [+-]infinity.
// Writes NaN, Infinity, and -Infinity.
//XXX writes float %g, with e+, reads e20...

// This doesn't read or write leading or trailing whitespace.
// This doesn't read or write hex or octal numbers.
// This doesn't write, but does read, leading +.
// This doesn't write, but does read, excess leading zeros.
// This doesn't set errno.
// This reads and writes correctly with rounding to nearest, ties to even.
// This uses minimal precision to make writing faithful, so, for example, 1.4
// is written rather than 1.39999999999996.

// Scan:
//   The input must be a null terminated string.
//   Returns the number of characters scanned.
//   Return true iff scan successful.
//   Characters may be used even if scan unsuccessful.
//   Invalid inputs (chars consumed, return false) include
//     overflows
//     lines without a newline termination
//   Valid inputs include
//     floating point underflows
//   The value is only touched on success.

// Format:
//   The only failure is a buffer overflow, in which case the buffer may be
//    touched.
//   Argument bufl is the length of buf.
//   Return true iff buf long enough for output and terminating null.
//   Argument outl can be 0, if not the output length is stored here (not
//   including the terminating null), even if buf is too short.

// The buffer lengths are a comfortable overestimate of the maximum length
// required.

// Reasons for not using strtod, sprintf/scanf, >>:
//   Some libraries not correct.
//   Some libraries horribly slow.
//   No libraries provide minimum precision floating-point representations.
//   Have to change locale for C functions.
//   C++ streams inefficient when writing to memory.
//   C++ streams have a state that effects the output, so to ensure the
//    intended output with an arbitrary stream, you must set this entire
//    state every time.

// simple_match performs a glob match, with just | and * magic characters.

// The std::string returning methods can throw std::bad_alloc (internally and
// from the std::string initialization).
// The other sprintf methods will visibly truncate the output instead of
// throwing an exception.

#include <cstdarg>
#include <string>
#include <vector>

namespace ew {
  class String {
  public:
    static const int FORMAT_INT_LEN = 24;
    static const int FORMAT_FLOAT_LEN = 24;
    static const int FORMAT_DOUBLE_LEN = 32;
    inline static bool is_nws(char c);
    inline static bool scan_ws(const char **se, const char *s);
    inline static bool scan_not_ws(const char **se, const char *s);
    inline static bool scan_ws_not_nl(const char **se,
     const char *s);
    inline static bool scan_line(const char **se, const char *s);
    static bool scan_int(int *val, const char **se, const char *s);
    static bool scan_finite_float(float *val, const char **se, const char *s);
    static bool scan_finite_double(double *val, const char **se,
     const char *s);
    static bool format_int(char *buf, int bufl, int *outl, int val);
    static bool format_float(char *buf, int bufl, int *outl, float val);
    static bool format_double(char *buf, int bufl, int *outl, double val);
    static bool simple_match(const char *pattern, const char *str);
    static char *vbvprintf(int *outl, bool *is_truncated, char *buf,
     int bufl, std::vector<char> &a, const char *msg, std::va_list vargs);
#ifdef __GNUC__
    static char *bvprintf(int *outl, bool *is_truncated, char *buf,
     int bufl, std::vector<char> &a, const char *msg, ...)
     __attribute((format(printf,6,7)));
#else
    static char *bvprintf(int *outl, bool *is_truncated, char *buf,
     int bufl, std::vector<char> &a, const char *msg, ...);
#endif
    static std::string vssprintf(const char *msg, std::va_list vargs);
#ifdef __GNUC__
    static std::string ssprintf(const char *msg, ...)
     __attribute((format(printf,1,2)));
#else
    static std::string ssprintf(const char *msg, ...);
#endif
    static int pathlen(const char *s);
  private:
    inline static bool scan_property(const char **se, const char *s,
     unsigned int prop);
    static const unsigned int PROP_WS    = 0x01;
    static const unsigned int PROP_NWS   = 0x02;
    static const unsigned int PROP_WSNNL = 0x04;
    static const unsigned int Z = 0;
    static const unsigned int C = ew::String::PROP_NWS;
    static const unsigned int S = ew::String::PROP_WS | ew::String::PROP_WSNNL;
    static const unsigned int N = ew::String::PROP_WS;
    static const unsigned int P = ew::String::PROP_NWS;
    static const unsigned int D = ew::String::PROP_NWS;
    static const unsigned int W = ew::String::PROP_NWS;
    static const unsigned int H = ew::String::PROP_NWS;
    static const unsigned int U = ew::String::PROP_NWS;
    static const unsigned int G = ew::String::PROP_NWS;
    static const unsigned int L = ew::String::PROP_NWS;
    static const unsigned int E = ew::String::PROP_NWS;
    static const unsigned char char_props[256];
  private:
// undefined
    String();
  };
}

inline bool
ew::String::is_nws(char c)
{
  return ((ew::String::char_props[static_cast<unsigned const char>(c)] &
   ew::String::PROP_NWS) != 0);
}

inline bool
ew::String::scan_property(const char **se, const char *s, unsigned int prop)
{
  const char *s0 = s;
  while (ew::String::char_props[static_cast<unsigned const char>(*s)] & prop) {
    s += 1;
  }
  if (se != 0) {
    *se = s;
  }
  return (s != s0);
}

inline bool
ew::String::scan_ws(const char **se, const char *s)
{
  return ew::String::scan_property(se, s, ew::String::PROP_WS);
}

inline bool
ew::String::scan_not_ws(const char **se, const char *s)
{
  return ew::String::scan_property(se, s, ew::String::PROP_NWS);
}

inline bool
ew::String::scan_ws_not_nl(const char **se, const char *s)
{
  return ew::String::scan_property(se, s, ew::String::PROP_WSNNL);
}

inline bool
ew::String::scan_line(const char **se, const char *s)
{
  while (1) {
    if (*s == '\n') {
      s += 1;
      if (se != 0) {
        *se = s;
      }
      return true;
    } else if (*s == 0) {
      if (se != 0) {
        *se = s;
      }
      return false;
    } else {
      s += 1;
    }
  }
}

#endif

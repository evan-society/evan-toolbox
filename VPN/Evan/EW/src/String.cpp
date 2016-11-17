// wdkg 2008-2010

// This assumes ieee 754 format single and double precisions floating point
// numbers.
// This assumes int is not more than 64 bits, and is two's complement with no
// padding.
// XXX use int32_t
// This uses a modified version of David Gay's gdtoa library, instead of the
// system printf, scanf, strtod or strtol.

#include <climits>
#include <cstdarg>
#include <cstdio>
#include <stdint.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "ew/String.h"
#include "Gdtoa.h"

namespace {
  const int BUF_SZ = 256;
}

const unsigned char ew::String::char_props[256] = {
  Z, C, C, C, C, C, C, C, C, S, N, S, S, S, C, C,
  C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C,
  S, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P,
  D, D, D, D, D, D, D, D, D, D, P, P, P, P, P, P,
  P, H, H, H, H, H, H, U, U, U, U, U, U, U, U, U,
  U, U, U, U, U, U, U, U, U, U, U, P, P, P, P, P,
  P, G, G, G, G, G, G, L, L, L, L, L, L, L, L, L,
  L, L, L, L, L, L, L, L, L, L, L, P, P, P, P, C,
  E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
  E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
  E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
  E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
  E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
  E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
  E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
  E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E
};

bool
ew::String::scan_int(int *val, const char **se, const char *s)
{
  const char *s0 = s;
  int neg = 0;
  if (*s == '+') {
    s += 1;
  } else if (*s == '-') {
    s += 1;
    neg = 1;
  }
  const char *s1 = s;
  unsigned int av = 0;
  int overflow = 0;
  while (1) {
    char c = *s;
    if (c < '0' || c > '9') {
      break;
    }
    if (av > UINT_MAX / 10 - 1) {
      overflow = 1;
    }
    c -= '0';
    av *= 10;
    av += c;
    s += 1;
  }
  if (s == s1) {
    if (se != 0) {
      *se = s0;
    }
    return false;
  }
  if (se != 0) {
    *se = s;
  }
  if (overflow) {
    return false;
  }
  if (neg) {
    if (av > -static_cast<unsigned int>(INT_MIN)) {
      return false;
    }
// Assume signed representation is two's complement.
    union {
      int i;
      unsigned int u;
    } un;
    un.u = -av;
    *val = un.i;
  } else {
    if (av > INT_MAX) {
      return false;
    }
    *val = static_cast<int>(av);
  }
  return true;
}

bool
ew::String::format_int(char *buf, int bufl, int *outl, int val)
{
  char rbuf[ew::String::FORMAT_INT_LEN];
// Absolute value, even for -2147483648.
  unsigned int uval = static_cast<unsigned int>(val);
  if (val < 0) {
    uval = -uval;
  }
  rbuf[0] = 0;
  int i = 0;
  if (uval == 0) {
    i += 1;
    rbuf[i] = '0';
  } else {
    while (uval > 0) {
      i += 1;
      rbuf[i] = '0' + (uval % 10);
      uval = uval / 10;
    }
  }
  if (val < 0) {
    i += 1;
    rbuf[i] = '-';
  }
  if (outl != 0) {
    *outl = i;
  }
  if (bufl > i) {
    while (i >= 0) {
      *buf = rbuf[i];
      buf += 1;
      i -= 1;
    }
    return true;
  } else {
    return false;
  }
}

bool
ew::String::scan_finite_float(float *val, const char **se, const char *s)
{
// Unlike gdtoa, we don't automatically swallow whitespace.
  if (!ew::String::is_nws(*s)) {
    if (se) {
      *se = s;
    }
    return false;
  }
  char *s1;
  float v;
  int fptype = ew::Gdtoa::strtopf(s, &s1, &v);
  if ((fptype & ew::Gdtoa::OVERFLOW) ||
   (fptype & ew::Gdtoa::RETMASK) == ew::Gdtoa::NAN ||
   (fptype & ew::Gdtoa::RETMASK) == ew::Gdtoa::NANBITS ||
   (fptype & ew::Gdtoa::RETMASK) == ew::Gdtoa::INFINITE) {
    if (se) {
      *se = s1;
    }
    return false;
  }
  if ((fptype & ew::Gdtoa::RETMASK) == ew::Gdtoa::NONUMBER) {
    if (se) {
      *se = s;
    }
    return false;
  }
  if (se) {
    *se = s1;
  }
  *val = v;
  return true;
}

bool
ew::String::format_float(char *buf, int bufl, int *outl, float val)
{
//XXX does ew::Gdtoa::ffmt behave correctly in corner cases of buffer length
  char *r = ew::Gdtoa::ffmt(buf, &val, 0, bufl - 1);
  if (r == 0) {
//XXX ew::Gdtoa::ffmt does not return the necessary length, so overestimate.
    if (outl) {
      *outl = ew::String::FORMAT_FLOAT_LEN;
    }
    return false;
  }
  if (outl) {
    *outl = r - buf;
  }
  return true;
}

bool
ew::String::scan_finite_double(double *val, const char **se, const char *s)
{
// Unlike gdtoa, we don't eat whitespace.
  if (!ew::String::is_nws(*s)) {
    if (se) {
      *se = s;
    }
    return false;
  }
  char *s1;
  double v;
  int fptype = ew::Gdtoa::strtopd(s, &s1, &v);
  if ((fptype & ew::Gdtoa::OVERFLOW) ||
   (fptype & ew::Gdtoa::RETMASK) == ew::Gdtoa::NAN ||
   (fptype & ew::Gdtoa::RETMASK) == ew::Gdtoa::NANBITS ||
   (fptype & ew::Gdtoa::RETMASK) == ew::Gdtoa::INFINITE) {
    if (se) {
      *se = s1;
    }
    return false;
  }
  if ((fptype & ew::Gdtoa::RETMASK) == ew::Gdtoa::NONUMBER) {
    if (se) {
      *se = s;
    }
    return false;
  }
  if (se) {
    *se = s1;
  }
  *val = v;
  return true;
}

bool
ew::String::format_double(char *buf, int bufl, int *outl, double val)
{
//XXX does ew::Gdtoa::dfmt behave correctly in corner cases of buffer length
  char *r = ew::Gdtoa::dfmt(buf, &val, 0, bufl - 1);
  if (r == 0) {
//XXX ew::Gdtoa::dfmt does not return the necessary length, so overestimate.
    if (outl) {
      *outl = ew::String::FORMAT_DOUBLE_LEN;
    }
    return false;
  }
  if (outl) {
    *outl = r - buf;
  }
  return true;
}

// This returns a pointer to the pattern branch terminator on failure, 0 on
// success.

namespace {

  const char *
  MatchBranch(const char *pattern, const char *str)
  {
    while (1) {
      if (*pattern == '*') {
        while (1) {
          pattern += 1;
          if (*pattern != '*') {
            break;
          }
        }
        if (*pattern == 0 || *pattern == '|') {
          return 0;
        }
        while (1) {
          if (*str == 0) {
            while (*pattern != 0 && *pattern != '|') {
              pattern += 1;
            }
            return pattern;
          } else if (*str == *pattern &&
           MatchBranch(pattern + 1, str + 1) == 0) {
            return 0;
          }
          str += 1;
        }
      } else if (*pattern == 0 || *pattern == '|') {
        if (*str == 0) {
          return 0;
        } else {
          return pattern;
        }
      } else if (*pattern == *str) {
        str += 1;
        pattern += 1;
      } else {
        while (*pattern != 0 && *pattern != '|') {
          pattern += 1;
        }
        return pattern;
      }
    }
    return NULL; //!?
  }

}

bool
ew::String::simple_match(const char *pattern, const char *str)
{
  if (pattern[0] == '*' && pattern[1] == 0) {
    return true;
  }
  while (1) {
    pattern = MatchBranch(pattern, str);
    if (pattern == 0) {
      return true;
    }
    if (*pattern == 0) {
      return false;
    }
    pattern += 1;
  }
  return false; //!?
}

char *
ew::String::vbvprintf(int *outl, bool *is_truncated, char *buf, int bufl,
 std::vector<char> &a, const char *msg, std::va_list vargs)
{
  int len = std::vsnprintf(buf, bufl, msg, vargs);
  if (len < bufl) {
    if (outl) {
      *outl = len;
    }
    if (is_truncated) {
      *is_truncated = false;
    }
    return buf;
  }
  try {
    a.resize(len + 1);
    std::vsnprintf(&(a[0]), len + 1, msg, vargs);
    if (outl) {
      *outl = len;
    }
    if (is_truncated) {
      *is_truncated = false;
    }
    return &(a[0]);
  } catch (std::exception &e) {
// could be a std::bad_alloc, or, less likely, a std::bad_length
// truncate rather than give up
    buf[bufl - 4] = '.';
    buf[bufl - 3] = '.';
    buf[bufl - 2] = '.';
    buf[bufl - 1] = 0;
    if (outl) {
      *outl = bufl - 1;
    }
    if (is_truncated) {
      *is_truncated = true;
    }
    return buf;
  }
}

char *
ew::String::bvprintf(int *outl, bool *is_truncated, char *buf, int bufl,
 std::vector<char> &a, const char *msg, ...)
{
  std::va_list vargs;
  va_start(vargs, msg);
  char *p = ew::String::vbvprintf(outl, is_truncated, buf, bufl, a, msg, vargs);
  va_end(vargs);
  return p;
}

std::string
ew::String::vssprintf(const char *msg, std::va_list vargs)
{
  bool is_truncated;
  std::vector<char> a;
  char buf[BUF_SZ];
  char *p = ew::String::vbvprintf(0, &is_truncated, buf, BUF_SZ, a, msg, vargs);
  if (is_truncated) {
    throw std::bad_alloc();
  }
  return p;
}

std::string
ew::String::ssprintf(const char *msg, ...)
{
// If we simply wrapped ew::String::vssprintf, an exception would cause va_end
// to be skipped.
  bool is_truncated;
  std::vector<char> a;
  char buf[BUF_SZ];
  std::va_list vargs;
  va_start(vargs, msg);
  char *p = ew::String::vbvprintf(0, &is_truncated, buf, BUF_SZ, a, msg, vargs);
  va_end(vargs);
  if (is_truncated) {
    throw std::bad_alloc();
  }
  return p;
}

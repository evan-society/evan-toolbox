// wdkg 2009-2010

#include <cstring>
#include "File.h"

namespace {
  const char PathSeparator = '/';
#ifdef _WIN32
  const char PathSeparator2 = '\\';
#endif
}

int
ew::File::pathlen(const char *s)
{
  const char *p = strrchr(s, PathSeparator);
#ifdef _WIN32
  const char *s1;
  if (p == 0) {
    s1 = s;
  } else {
    s1 = p;
  }
  const char *p2 = strrchr(s1, PathSeparator2);
  if (p2 != 0) {
    p = p2;
  }
#endif
  int l;
  if (p == 0) {
    l = 0;
  } else {
    l = (p - s) + 1;
  }
  return l;
}

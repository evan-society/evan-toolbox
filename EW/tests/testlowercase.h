#ifndef EWTST_TEST_H_INCLUDED
#define EWTST_TEST_H_INCLUDED

// wdkg 2009-2010

#include <iostream>
#include <string>

typedef struct {
  const char *test_n;
  void (*test_f)();
} test_t;

extern int ProcessCmdline(const char name[], const char desc[],
 const char additional_usage[], test_t *tests, int ac, char **av);
extern std::string FullPath(const char *tail);
extern void Error(const char format[], ...);
extern int TestN;
extern bool TestGuiMode;
extern std::ostream *TestOut;
extern const char *TestDir;

#endif

// wdkg 2009-2010

#ifndef TESTDIR
#error "TESTDIR undefined"
#endif

#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <stdint.h>
#include <string>
#include <stdexcept>
#include "ew/String.h"
#include "ew/Version.h"
#include "Test.h"

namespace {
  const char TestDir[] = TESTDIR;
  const char PathSeparator = '/';
  const char Author[] = "wdkg";
  const char Options[] = "n_l";
  const uint32_t R_SEED_BASE = 123459876;
  const uint32_t R_MERSENNE_31 = 2147483647;
  const uint32_t R_MULTIPLIER = 16807;
// R_MERSENNE_31 = R_MULTIPLIER * R_QUOTIENT + R_REMAINDER
  const uint32_t R_QUOTIENT = 127773;
  const uint32_t R_REMAINDER = 2836;
}

ewtst::Test::Test(const char *i_name, const char *i_desc,
 const char *i_debug_val) :
 name(i_name),
 desc(i_desc),
 debug_val(i_debug_val),
 seed(R_SEED_BASE),
 errs(0)
{
}

// This is the "minimal standard" algorithm of Park and Miller.
//     S.K. Park & K.W. Miller
//     "Random number generators: good ones are hard to find,"
//     Comm ACM 31(10):1192-1201, Oct 1988

void
ewtst::Test::random_seed(int v)
{
  seed = R_SEED_BASE + v;
}

uint32_t
ewtst::Test::random_int()
{
// This calculates (seed * R_MULTIPLIER) % R_MERSENNE_31 with 32 bit arithmetic.
  uint32_t a = R_MULTIPLIER * (seed % R_QUOTIENT);
  uint32_t b = R_REMAINDER * (seed / R_QUOTIENT);
  if (b > a) {
    seed = a + (R_MERSENNE_31 - b);
  } else {
    seed = a - b;
  }
  return seed;
}

double
ewtst::Test::random_double()
{
  return random_int() * (1.0 / R_MERSENNE_31);
}

std::string
ewtst::Test::full_path(const char *tail)
{
  return ew::String::ssprintf("%s%c%s", TestDir, PathSeparator, tail);
}

void
ewtst::Test::report_error(const char format[], ...)
{
  errs += 1;
  va_list vargs;
  va_start(vargs, format);
  std::printf("  error: ");
  std::vprintf(format, vargs);
  std::printf("\n");
  va_end(vargs);
}

int
ewtst::Test::exec_base(int ac, char **av)
{
  if (ac == 2 && std::strcmp(av[1], "--version") == 0) {
    std::printf("%s %s %s\n", name, Author, ew::Version::version_str());
    return 0;
  }
  if (ac == 2 &&
   (std::strcmp(av[1], "-h") == 0 || std::strcmp(av[1], "--help") == 0)) {
// Cannot use a const char * for the format because it disables gcc type safety.
    std::printf(
     "%s"
     "Usage:\n"
     "  %s\n"
     "  %s -n <n>\n"
     "  %s -l\n"
     "Options:\n"
     "  -n  run just the <n>'th test\n"
     "  -l  list the tests\n",
     desc, name, name, name);
    if (debug_val != 0) {
      std::printf(
       "Environmental Variables:\n"
       "  EW_DEBUG_PATTERN=%s\n",
       debug_val);
    }
    std::printf("Installation directory: %s\n", TestDir);
    return 0;
  }
  bool option_ons[128];
  const char *option_vals[128];
  for (int i = 0; i < 128; i += 1) {
    option_ons[i] = false;
    option_vals[i] = 0;
  }
  while (ac > 1 && av[1][0] == '-' && av[1][1] != 0) {
    ac -= 1;
    av += 1;
    if (av[0][1] == '-' && av[0][2] == 0) {
      break;
    }
    const char *p = av[0] + 1;
    while (p[0] != 0) {
      bool got_option = false;
      for (int i = 0; Options[i] != 0; i += 1) {
        if (p[0] == Options[i] && Options[i] != '_') {
          option_ons[p[0]] = true;
          if (Options[i + 1] == '_') {
            const char *val;
            if (p[1] != 0) {
              val = p + 1;
              while (p[0] != 0) {
                p += 1;
              }
            } else {
              if (ac <= 1) {
                std::fprintf(stderr, "%s: fatal error, bad usage\n", name);
                return 1;
              }
              val = av[1];
              ac -= 1;
              av += 1;
            }
            option_vals[p[0]] = val;
          }
          p += 1;
          got_option = true;
          break;
        }
      }
      if (!got_option) {
        std::fprintf(stderr, "%s: fatal error, bad usage\n", name);
        return 1;
      }
    }
  }
  ac -= 1;
  av += 1;
  if (ac != 0 || (option_ons['l'] && option_ons['n'])) {
    std::fprintf(stderr, "%s: fatal error, bad usage\n", name);
    return 1;
  }
  int num_tests = 0;
  while (test(1 + num_tests, true) != 0) {
    num_tests += 1;
  }
  int testno;
  char ch;
  if (option_ons['n']) {
    if (std::sscanf(option_vals['n'], "%d%c", &testno, &ch) != 1) {
      std::fprintf(stderr, "%s: fatal error, bad usage\n", name);
      return 1;
    }
    if (testno < 1 || testno > num_tests) {
      std::fprintf(stderr, "%s: fatal error, bad test number\n", name);
      return 1;
    }
  }
  if (option_ons['l']) {
    for (int i = 1; i <= num_tests; i += 1) {
      std::printf("%2d %s\n", i, test(i, true));
    }
    return 0;
  }
  std::string chkname = full_path("test_dataflow.cpp");
  std::FILE *fd = std::fopen(chkname.c_str(), "rb");
  if (fd == 0) {
    std::fprintf(stderr, "%s: fatal error, bad TESTDIR %s\n", name, TestDir);
    return 1;
  }
  std::fclose(fd);
  std::string okname = TestDir;
  okname += PathSeparator;
  okname += name;
  okname += ".ok";
  remove(okname.c_str());
  try {
    if (option_ons['n']) {
      std::printf("%s: testing %d %s\n", name, testno, test(testno, true));
      random_seed(testno);
      test(testno, false);
    } else {
      for (int i = 1; i <= num_tests; i += 1) {
        std::printf("%s: testing %d %s\n", name, i, test(i, true));
        random_seed(i);
        test(i, false);
      }
    }
  } catch (std::logic_error &e) {
    std::fprintf(stderr, "%s: fatal error, logic_error %s\n", name, e.what());
    return 1;
  } catch (std::runtime_error &e) {
    std::fprintf(stderr, "%s: fatal error, runtime_error %s\n", name, e.what());
    return 1;
  } catch (std::exception &e) {
    std::fprintf(stderr, "%s: fatal error, std::exception %s\n", name,
     e.what());
    return 1;
  } catch (...) {
    std::fprintf(stderr, "%s: fatal error, unknown exception\n", name);
    return 1;
  }
  std::printf("%s: completed with %d errors\n", name, errs);
  if (errs > 0) {
    return 1;
  }
  if (!option_ons['n']) {
    FILE *fd = std::fopen(okname.c_str(), "wb");
    if (fd == 0) {
      std::fprintf(stderr, "%s: fatal error, cannot touch %s\n", name,
       okname.c_str());
      return 1;
    }
    std::fclose(fd);
  }
  return 0;
}

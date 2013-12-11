//XXX transition to Test.cpp
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "testlowercase.h"

#ifndef TESTDIR
#error "TESTDIR undefined"
#endif

static int Errs = 0;

int TestN;
bool TestGuiMode;
std::ostream *TestOut;
const char *TestDir = TESTDIR;
const char PathSeparator = '/';

std::string
FullPath(const char *tail)
{
  std::string s = TestDir;
  s += PathSeparator;
  s += tail;
  return s;
}

int
ProcessCmdline(const char name[], const char desc[],
 const char additional_usage[], test_t *tests, int ac, char **av)
{
  int testno;
  if (ac == 1) {
    TestGuiMode = true;
    testno = -1;
  } else if (ac == 2 && std::strcmp(av[1], "-h") == 0) {
    std::cout << desc <<
     "Usage:\n" <<
     "  " << name << " -h           usage\n" <<
     "  " << name << " -l           list tests\n" <<
     "  " << name << " -a           run all tests\n" <<
     "  " << name << " -n <n>       run <n>'th test\n";
    if (additional_usage) {
      std::cout << additional_usage;
    }
    std::cout << "Installation directory: " << TestDir << "\n";
    return 0;
  } else if (ac == 2 && std::strcmp(av[1], "-l") == 0) {
    for (int i = 0; tests[i].test_f; i += 1) {
      std::streamsize w = std::cout.width(2);
      std::cout << i + 1 << " ";
      std::cout.width(w);
      std::cout << tests[i].test_n << "\n";
    }
    return 0;
  } else if (ac == 2 && std::strcmp(av[1], "-a") == 0) {
    TestGuiMode = false;
    testno = -1;
  } else if (ac == 3 && std::strcmp(av[1], "-n") == 0) {
    TestGuiMode = false;
    char ch;
    if (std::sscanf(av[2], "%d%c", &testno, &ch) != 1) {
      std::cout << name << ": fatal error, bad usage\n";
      return 1;
    }
    int i;
    for (i = 0; tests[i].test_f; i += 1) {
    }
    if (testno > i || testno < 1) {
      std::cout << name << ": fatal error, bad test number\n";
      return 1;
    }
  } else {
    std::cout << name << ": fatal error, bad usage\n";
    return 1;
  }
  std::string chkname = FullPath("test_dataflow.cpp");
  std::ifstream chkfile;
  chkfile.open(chkname.c_str());
  if (!chkfile.is_open()) {
// If TestGuiMode, nowhere to send message.
    if (!TestGuiMode) {
      std::cout << name << ": fatal error, bad TESTDIR " << TestDir << "\n";
    }
    return 1;
  }
  chkfile.close();
  std::ofstream outp;
  if (TestGuiMode) {
    std::string outname = FullPath("test.out");
    outp.open(outname.c_str(), std::ios_base::app);
    if (!outp.is_open()) {
// Nowhere to send error message.
      return 1;
    }
    TestOut = &outp;
  } else {
    TestOut = &std::cout;
  }
  std::string okname = TestDir;
  okname += PathSeparator;
  okname += name;
  okname += ".ok";
  remove(okname.c_str());
  try {
    for (int i = 0; tests[i].test_f; i += 1) {
      if (testno < 0 || testno == i + 1) {
        TestN = i + 1;
        *TestOut << name << ": testing " << i + 1 << " " << tests[i].test_n <<
         std::endl;
        (*tests[i].test_f)();
      }
    }
  } catch (std::logic_error &e) {
    *TestOut << name << ": fatal error, logic error " << e.what() << "\n";
    return 1;
  } catch (std::runtime_error &e) {
    *TestOut << name << ": fatal error, runtime error " << e.what() << "\n";
    return 1;
  } catch (std::exception &e) {
    *TestOut << name << ": fatal error, std exception " << e.what() << "\n";
    return 1;
  } catch (...) {
    *TestOut << name << ": fatal error, unknown exception\n";
    return 1;
  }
  *TestOut << name << ": completed with " << Errs << " errors\n";
  if (Errs == 0 && testno == -1) {
    std::ofstream ok;
    ok.open(okname.c_str());
    if (ok.is_open()) {
      ok.close();
    } else {
      *TestOut << name << ": cannot touch " << okname << "\n";
    }
  }
  if (TestGuiMode) {
    outp.close();
  }
  if (Errs > 0) {
    return 1;
  } else {
    return 0;
  }
}

void
Error(const char format[], ...)
{
  Errs += 1;
  va_list vargs;
  va_start(vargs, format);
  char buf[1024];
  std::vsnprintf(buf, 1024, format, vargs);
  *TestOut << "  error: " << buf << std::endl;
  va_end(vargs);
}

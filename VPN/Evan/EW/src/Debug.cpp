// wdkg 2008-2010
//XXX use stdio instead of iostream for thread safety

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "ew/Debug.h"
#include "ew/ErrorRuntime.h"
#include "ew/String.h"
#include "ew/Time.h"
#include "ew/Version.h"

// Don't filter ew::Debug messages with EW_DEBUG_PATTERN.
// Cannot use debug enabled forms of exceptions before debug system start up!

namespace {
  const char ClassName[] = "ew::Debug";
  const char EnvFile[] = "EW_DEBUG_FILE";
  const char EnvPattern[] = "EW_DEBUG_PATTERN";
  const int BUFSZ1 = 16;
  const int BUFSZ2 = 400;
  const int TEST_LINE_LEN = 80;
  const char ErrorExists[] =
   "The debug message output file exists but does not look like a debug"
   " message output file.";
  const char ErrorOpen[] =
   "The debug message output file could not be opened.";
  const char ErrorWrite[] =
   "The debug message output file could not be written to.";
}

ew::Debug::Debug() :
 active(false),
 is_file(false),
 alerts(false),
 failures(false)
// pattern uninitialized
// file_str uninitialized
// str uninitialized
{
  const char *pat = getenv(EnvPattern);
  const char *file = getenv(EnvFile);
  if (file == 0 && pat == 0) {
// No debugging, just start the timer.
    ew::Time::elapsed();
    return;
  }
  if (pat == 0) {
    pattern = "*";
  } else {
    pattern = pat;
  }
  if (file == 0 || file[0] == 0 || (file[0] == '-' && file[1] == 0)) {
    str = &std::cout;
  } else {
// For safety, if file exists, check it contains ew::Debug output.
    std::ifstream test(file);
    if (test.is_open()) {
      char buf[TEST_LINE_LEN + 2];
      test.getline(buf, TEST_LINE_LEN);
      if (std::strncmp(buf, "0.000000 ew::Debug::CTOR", 24) != 0) {
        throw std::runtime_error(ErrorExists);
      }
      test.close();
    }
    file_str.open(file, (std::ios_base::app | std::ios_base::out));
    if (!file_str.is_open()) {
      throw std::runtime_error(ErrorOpen);
    }
    str = &file_str;
    is_file = true;
  }
  active = true;
// This starts ew::Time, and outputs the version string.
// This embeds the version string in the library and executable binaries, even
// if this code is not reached.
  dprintf("%s::CTOR(pattern=\"%s\", %s) {}", ClassName, pattern,
   ew::Version::id_str());
  if (str->fail()) {
    active = false;
    throw std::runtime_error(ErrorWrite);
  }
}

ew::Debug::~Debug()
{
  if (active) {
    try {
      if (failures) {
        if (alerts) {
          dprintf("%s::DTOR(FAILURES!!!) {}", ClassName);
        } else {
          dprintf("%s::DTOR(ALERTS!!!, FAILURES!!!) {}", ClassName);
        }
      } else {
        if (alerts) {
          dprintf("%s::DTOR(ALERTS!!!) {}", ClassName);
        } else {
          dprintf("%s::DTOR() {}", ClassName);
        }
      }
    } catch (std::exception &e) {
// Discard final debug message exceptions or errors.
    	dprintf( "%s\n", e.what() ); //?
    }
  }
  if (is_file) {
    try {
      file_str.close();
    } catch (std::exception &e) {
// Discard file close exceptions or errors.
    	dprintf( "%s\n", e.what() ); //?
    }
  }
}

bool
ew::Debug::get_active_helper(const char *classname) const
{
  return ew::String::simple_match(pattern, classname);
}

bool
ew::Debug::dprintf(const char *msg, ...) const
{
  std::va_list vargs;
  va_start(vargs, msg);
  dvprintf(msg, vargs);
  va_end(vargs);
  return true; //!?
}

bool
ew::Debug::dvprintf(const char *msg, va_list vargs) const
{
  if (!active) {
    return true;
  }
  if (msg[0] == '!') {
    alerts = true;
  }
  double t = ew::Time::elapsed();
  char buf1[BUFSZ1];
  std::vector<char> a1;
  int len1;
  char *p1 = ew::String::bvprintf(&len1, 0, buf1, BUFSZ1, a1, "%.6f ", t);
  char buf2[BUFSZ2];
  std::vector<char> a2;
  int len2;
  char *p2 = ew::String::vbvprintf(&len2, 0, buf2, BUFSZ2, a2, msg,
   vargs);
  try {
    str->write(p1, len1);
    str->write(p2, len2);
    str->write("\n", 1);
    str->flush();
  } catch (std::exception &e) {
    failures = true;
  }
  if (str->fail()) {
    failures = true;
  }
  return true;
}

const char *
ew::Debug::to_str(bool v)
{
  if (v) {
    return "true";
  } else {
    return "false";
  }
}

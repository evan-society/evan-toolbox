// wdkg 2009-2010

#include <stdexcept>
#include <string>
#include "ew/Debug.h"
#include "ew/ErrorLogic.h"
#include "ew/String.h"

//XXX reimplement what instead of building string on construction
namespace {
  const char ClassName[] = "ew::ErrorLogic";
  const char ErrorStart[] = "A programming error has been detected at";
}

ew::ErrorLogic::ErrorLogic(const char *filename, int lineno) :
 std::logic_error(ew::String::ssprintf("%s %s:%d.", ErrorStart, filename,
 lineno))
{
  const ew::Debug &dbg = ew::Debug::instance();
  dbg.get_active(ClassName) && dbg.dprintf("%s: %s", ClassName, what());
}

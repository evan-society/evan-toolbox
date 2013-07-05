// wdkg 2010

#include <stdexcept>
#include <string>
#include "ew/Debug.h"
#include "ew/ErrorIO.h"

namespace {
  const char ClassName[] = "ew::ErrorIO";
}

/// Construct an ew::ErrorIO object.
/// @param desc the error description

ew::ErrorIO::ErrorIO(const std::string &desc) :
 std::runtime_error(desc)
{
  const ew::Debug &dbg = ew::Debug::instance();
  dbg.get_active(ClassName) && dbg.dprintf("%s: %s", ClassName, desc.c_str());
}

// wdkg 2010

#include <stdexcept>
#include <string>
#include "ew/Debug.h"
#include "ew/ErrorRuntime.h"

namespace {
  const char ClassName[] = "ew::ErrorRuntime";
}

/// Construct an EW::Error_Runtime object.
/// @param desc the error description

ew::ErrorRuntime::ErrorRuntime(const std::string &desc) :
 std::runtime_error(desc)
{
  const ew::Debug &dbg = ew::Debug::instance();
  dbg.get_active(ClassName) && dbg.dprintf("%s: %s", ClassName, desc.c_str());
}

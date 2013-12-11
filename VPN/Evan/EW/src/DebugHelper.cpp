// wdkg 2010

#include <cstdarg>
#include <cstring>
#include <string>
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/String.h"

ew::DebugHelper::DebugHelper(const char *i_cn,
 std::string instance_name_i) :
 debug(ew::Debug::instance()),
 cn(i_cn),
 instance_name(instance_name_i),
 in(instance_name.c_str()),
 full_name(ew::String::ssprintf("%s[%s]", i_cn, in)),
 fn(full_name.c_str()),
 on(debug.get_active(i_cn))
{
}

ew::DebugHelper::DebugHelper(const char *i_cn,
 const ew::DebugHelper &helper) :
 debug(ew::Debug::instance()),
 cn(i_cn),
 instance_name(),
 in(helper.in),
 full_name(ew::String::ssprintf("%s[%s]", i_cn, in)),
 fn(full_name.c_str()),
 on(debug.get_active(i_cn))
{
}

bool
ew::DebugHelper::dprintf(const char *msg, ...) const
{
  std::va_list vargs;
  va_start(vargs, msg);
  debug.dvprintf(msg, vargs);
  va_end(vargs);
  return true;
}

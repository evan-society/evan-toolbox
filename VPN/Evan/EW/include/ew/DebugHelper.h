#ifndef EW_DEBUGHELPER_H_INCLUDED
#define EW_DEBUGHELPER_H_INCLUDED

// wdkg 2010

// This header file is an internal part of the library.
// The class is not part of the library interface.
// This is used for member variables in classes in the interface.
// These members are public but not themselves part of the interface.

// Debug Message per-class utility

// This is a convience wrapper of debug related variables, intended to be a
// member of a class that needs debug capability.

// The instance_id argument is a string rather than a char pointer, so that it
// can be dynamically created.
// Don't encapsulate check of dbg.on into dprintf, so that low cost of
// debugging when turned off is not dependent on optimization.

// Keep debug statements short: cn is short for classname, in for
// instancename, fn for fullname.
// The fullname is the classname if there is no instancename, otherwise
// classname[instancename].

#include <string>
#include "ew/Debug.h"

namespace ew {

  class DebugHelper {
  public:
    inline DebugHelper(const char *i_cn);
    DebugHelper(const char *i_cn, std::string i_instance_name);
    DebugHelper(const char *i_cn, const ew::DebugHelper &helper);
    const ew::Debug& debug;
    const char *const cn;
// Alteration of public/private sections to allow const initialization.
  private:
    const std::string instance_name;
  public:
    const char *const in;
  private:
    const std::string full_name;
  public:
    const char *const fn;
    const bool on;
#ifdef __GNUC__
    bool dprintf(const char *msg, ...) const
     __attribute((format(printf,2,3),nothrow));
#else
    bool dprintf(const char *msg, ...) const;
#endif
// undefined
    DebugHelper(const ew::DebugHelper &that);
    DebugHelper &operator=(const ew::DebugHelper &that);
  };

}

inline
ew::DebugHelper::DebugHelper(const char *i_cn) :
 debug(ew::Debug::instance()),
 cn(i_cn),
 instance_name(),
 in(0),
 full_name(),
 fn(i_cn),
 on(debug.get_active(i_cn))
{
}

#endif

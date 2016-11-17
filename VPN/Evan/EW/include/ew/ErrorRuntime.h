#ifndef EW_ERRORRUNTIME_H_INCLUDED
#define EW_ERRORRUNTIME_H_INCLUDED

// wdkg 2010

#include <stdexcept>

namespace ew {
  class ErrorRuntime : public std::runtime_error {
  public:
    explicit ErrorRuntime(const std::string &desc);
  };
}

/// @class ew::ErrorRuntime
/// @headerfile ew/ErrorRuntime.h "ew/ErrorRuntime.h"
/// @brief Runtime Exception
///
/// ew::ErrorRuntime is an exception thrown by EW library classes to report
/// runtime errors.
///
/// ew::ErrorRuntime is a trivial extension of std::runtime_error.

#endif

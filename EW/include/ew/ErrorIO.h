#ifndef EW_ERRORIO_H_INCLUDED
#define EW_ERRORIO_H_INCLUDED

// wdkg 2010

#include <stdexcept>

namespace ew {
  class ErrorIO : public std::runtime_error {
  public:
    explicit ErrorIO(const std::string &desc);
  };
}

/// @class ew::ErrorIO
/// @headerfile ew/ErrorIO.h "ew/ErrorIO.h"
/// @brief I/O Exception
///
/// ew::ErrorIO is an exception thrown by EW library classes to report input
/// or output errors or bad file formats.
///
/// ew::ErrorIO is a trivial extension of std::runtime_error.

#endif

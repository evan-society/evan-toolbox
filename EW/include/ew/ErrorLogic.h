#ifndef EW_ERRORLOGIC_H_INCLUDED
#define EW_ERRORLOGIC_H_INCLUDED

// wdkg 2010

// This header file is an internal part of the library.
// The class is not part of the library interface.
// This is a utility, included with the public headers purely as a convenience.

// Logic Error Exception with more convenient constructor.

// The constructor can throw bad_alloc.

#include <stdexcept>

namespace ew {
  class ErrorLogic : public std::logic_error {
  public:
    ErrorLogic(const char *filename, int lineno);
  };
}

#endif

#ifndef EW_FILE_H_INCLUDED
#define EW_FILE_H_INCLUDED

// wdkg 2009-2010

// File and filename utilities.

// File path strategy:
//   use '/' as a separator
//   recognize '\\' and '/' as a separator on _WIN32
//   assume paths are utf-8 XXXon _WIN32
//   ignore paths like A:tt on _WIN32

namespace ew {
  class File {
  public:
    static int pathlen(const char *s);
  private:
// undefined
    File();
  };
}

#endif

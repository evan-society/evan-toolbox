#ifndef EW_VERSION_H_INCLUDED
#define EW_VERSION_H_INCLUDED

// wdkg 2010

// This header file is an internal part of the library.
// The class is not part of the library interface.
// This allows the version of the library to be determined from the include
// directory.

// This class is implemented entirely in this header.

#define EW_MAJOR_VERSION 4
#define EW_MINOR_VERSION 20101209

namespace ew {
  class Version {
  public:
    inline static const char *version_str();
    inline static const char *id_str();
  private:
// undefined
    Version();
  };
}

inline const char *
ew::Version::version_str()
{
  return id_str() + 11;
}

inline const char *
ew::Version::id_str()
{
  static const char IdStr[] = "EW_VERSION=4.20101209";
  return IdStr;
}

#endif

#ifndef EW_XMLAUTOWRIPTR_H_INCLUDED
#define EW_XMLAUTOWRIPTR_H_INCLUDED

// wdkg 2010

// This class is implemented entirely in this header.

// Auto xmlWri pointer.

#include <libxml/xmlwriter.h>

namespace ew {
  class XmlAutoWriPtr {
  public:
    inline XmlAutoWriPtr();
    inline ~XmlAutoWriPtr();
    inline void open_file(const char *f, int compress);
    inline operator xmlTextWriter *();
  private:
    xmlTextWriter *ptr;
// undefined
    XmlAutoWriPtr(const ew::XmlAutoWriPtr &that);
    XmlAutoWriPtr &operator=(const ew::XmlAutoWriPtr &that);
  };
}

inline
ew::XmlAutoWriPtr::XmlAutoWriPtr() :
 ptr(0)
{
}

inline
ew::XmlAutoWriPtr::~XmlAutoWriPtr()
{
  if (ptr != 0) {
    xmlFreeTextWriter(ptr);
  }
}

inline void
ew::XmlAutoWriPtr::open_file(const char *f, int compress)
{
  ptr = xmlNewTextWriterFilename(f, compress);
}

inline
ew::XmlAutoWriPtr::operator xmlTextWriter *()
{
  return ptr;
}

#endif

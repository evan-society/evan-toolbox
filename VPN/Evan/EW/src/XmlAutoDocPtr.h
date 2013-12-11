#ifndef EW_XMLAUTODOCPTR_H_INCLUDED
#define EW_XMLAUTODOCPTR_H_INCLUDED

// wdkg 2010

// This class is implemented entirely in this header.

// Auto xmlDoc pointer.

#include <libxml/parser.h>
#include <libxml/tree.h>

namespace ew {

  class XmlAutoDocPtr {
  public:
    inline XmlAutoDocPtr();
    inline ~XmlAutoDocPtr();
    inline void read_file(const char *f, int opts);
    inline operator xmlDoc *();
  private:
    xmlDoc *ptr;
// undefined
    XmlAutoDocPtr(const ew::XmlAutoDocPtr &that);
    XmlAutoDocPtr &operator=(const ew::XmlAutoDocPtr &that);
  };

}

inline
ew::XmlAutoDocPtr::XmlAutoDocPtr() :
 ptr(0)
{
}

inline
ew::XmlAutoDocPtr::~XmlAutoDocPtr()
{
  if (ptr != 0) {
    xmlFreeDoc(ptr);
  }
}

inline void
ew::XmlAutoDocPtr::read_file(const char *f, int opts)
{
  ptr = xmlReadFile(f, 0, opts);
}

inline
ew::XmlAutoDocPtr::operator xmlDoc *()
{
  return ptr;
}

#endif

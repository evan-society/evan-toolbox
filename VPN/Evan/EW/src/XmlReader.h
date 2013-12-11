#ifndef EW_XMLREADER_H_INCLUDED
#define EW_XMLREADER_H_INCLUDED

// wdkg 2009-2010

// Utilities to simplify reading xml files using the libxml2 library.

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "ew/ErrorIO.h"
#include "ew/Transform3.h"
#include "XmlAutoDocPtr.h"

namespace ew {

  class XmlReader {
  public:
    static const int BUFSZ = 256;
    inline xmlNode *get_children();
    XmlReader(const char *i_file, const char *i_type, const char *version);
    void read_attribute(std::string *val, const char *key, xmlNode *node);
    void read_attribute_n(int *n, const char *key, xmlNode *node, int def);
    void read_attribute_list(int *val, const char *key,
     const char *const *options, xmlNode *node, int def);
    void read_attribute_filename(std::string *val, const char *key,
     xmlNode *node);
    void read_ints(int *dd, int l, xmlNode *node);
    void read_doubles(double *dd, int l, xmlNode *node);
    void read_transform3(ew::Transform3 *tr, xmlNode *node);
    void read_list(int *val, const char *const *choices, xmlNode *node);
    ew::ErrorIO err_desc(const char *desc);
    ew::ErrorIO err_desc_line(const char *desc, int line);
    ew::ErrorIO err_key_line(const char *key, int line);
  private:
    ew::XmlAutoDocPtr doc;
    xmlNode *root;
    xmlNode *children;
    char err_buf[BUFSZ];
    const char *file;
    const char *type;
// undefined
    XmlReader(const ew::XmlReader &that);
    XmlReader &operator=(const ew::XmlReader &that);
  };

}

inline xmlNode *
ew::XmlReader::get_children()
{
  return children;
}

#endif

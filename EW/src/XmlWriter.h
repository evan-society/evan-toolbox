#ifndef EW_XMLWRITER_H_INCLUDED
#define EW_XMLWRITER_H_INCLUDED

// wdkg 2009-2010

// Utilities to simplify writing xml files using the libxml2 library.

#include <libxml/xmlwriter.h>
#include "ew/ErrorIO.h"
#include "ew/Transform3.h"
#include "XmlAutoWriPtr.h"

namespace ew {

  class XmlWriter {
  public:
    static const int BUFSZ = 256;
    XmlWriter(const char *i_file, const char *i_type, const char *version,
     bool compress);
    void close();
    ew::ErrorIO err_desc(const char *desc);
    ew::ErrorIO err_write();
    void start_element(const char *name);
    void end_element();
    void write_attribute(const char *val, const char *key);
    void write_attribute_n(int val, const char *key, int def);
    void write_attribute_list(int i, const char *const *options, int noptions,
     const char *key, int def);
    void write_attribute_filename(const char *val, const char *key);
    inline void write_string(const char *s);
    void write_int(int val);
    void write_ints(const int *dd, int n, int stride);
    void write_double(double val);
    void write_doubles(const double *dd, int n, int stride);
    void write_transform3(const ew::Transform3 *tr);
  private:
    ew::XmlAutoWriPtr wri;
    char err_buf[BUFSZ];
    const char *file;
    const char *type;
// undefined
    XmlWriter(const ew::XmlWriter &that);
    XmlWriter &operator=(const ew::XmlWriter &that);
  };

}

inline void
ew::XmlWriter::write_string(const char *s)
{
  if (xmlTextWriterWriteString(wri, reinterpret_cast<const xmlChar *>(s)) <
   0) {
    throw err_write();
  }
}

#endif

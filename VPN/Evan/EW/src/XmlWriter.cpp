// wdkg 2009-2010

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <libxml/xmlwriter.h>
#include "ew/Debug.h"
#include "ew/ErrorLogic.h"
#include "ew/ErrorIO.h"
#include "ew/String.h"
#include "ew/Transform3.h"
#include "File.h"
#include "XmlAutoWriPtr.h"
#include "XmlWriter.h"

namespace {
  const char ClassName[] = "ew::XmlWriter";

  inline const xmlChar *
  CastCxp(const char *p)
  {
    return reinterpret_cast<const xmlChar *>(p);
  }

  void
  XmlErrorHandler(void *ctx, const char *msg, ...)
  {
// cannot grow or throw, so don't use ew::String::format
    std::va_list vargs;
    va_start(vargs, msg);
    char *buf = static_cast<char *>(ctx);
    fflush(stdout);
// silently ignore overwrites
// silently truncate
    if (buf[0] == 0) {
      std::vsnprintf(buf, ew::XmlWriter::BUFSZ, msg, vargs);
    }
    va_end(vargs);
  }

}

ew::XmlWriter::XmlWriter(const char *f, const char *ty, const char *version,
 bool compress) :
// err_buf uninitialized
 file(f),
 type(ty)
{
  err_buf[0] = 0;
  xmlSetGenericErrorFunc(err_buf, XmlErrorHandler);
  if (compress) {
    wri.open_file(f, 1);
  } else {
    wri.open_file(f, 0);
  }
  if (wri == 0) {
    throw err_desc("the file could not be opened");
  }
  if (xmlTextWriterStartDocument(wri, 0, "UTF-8", 0) < 0) {
    throw err_write();
  }
  start_element(ty);
  write_attribute(version, "version");
  write_string("\n\n");
}

ew::ErrorIO
ew::XmlWriter::err_desc(const char *desc)
{
  if (err_buf[0] != 0) {
    return ew::ErrorIO(ew::String::ssprintf(
     "The %s file %s could not be saved (%s, %s).", type, file, desc,
     err_buf));
  } else {
    return ew::ErrorIO(ew::String::ssprintf(
     "The %s file %s could not be saved (%s).", type, file, desc));
  }
}

ew::ErrorIO
ew::XmlWriter::err_write()
{
  if (err_buf[0] != 0) {
    return ew::ErrorIO(ew::String::ssprintf(
     "The %s file %s could not be saved"
     " (the content could not be written, %s).", type, file, err_buf));
  } else {
    return ew::ErrorIO(ew::String::ssprintf(
     "The %s file %s could not be saved"
     " (the content could not be written).", type, file));
  }
}

void
ew::XmlWriter::close()
{
  end_element();
  if (xmlTextWriterEndDocument(wri) < 0) {
    throw err_desc("the file could not be closed");
  }
}

void
ew::XmlWriter::start_element(const char *name)
{
  if (xmlTextWriterStartElement(wri, CastCxp(name)) < 0) {
    throw err_write();
  }
}

void
ew::XmlWriter::end_element()
{
  if (xmlTextWriterEndElement(wri) < 0) {
    throw err_write();
  }
}

void
ew::XmlWriter::write_attribute(const char *val, const char *key)
{
  if (val[0] != 0) {
    if (xmlTextWriterWriteAttribute(wri, CastCxp(key),
     CastCxp(val)) < 0) {
      throw err_write();
    }
  }
}

void
ew::XmlWriter::write_attribute_n(int val, const char *key, int def)
{
  if (val == def) {
    return;
  }
  char buf[ew::String::FORMAT_INT_LEN];
  ew::String::format_int(buf, ew::String::FORMAT_INT_LEN, 0, val);
  if (xmlTextWriterWriteAttribute(wri, CastCxp(key), CastCxp(buf)) < 0) {
    throw err_write();
  }
}

void
ew::XmlWriter::write_attribute_list(int i, const char *const *options,
 int noptions, const char *key, int def)
{
  if (i < 0 || i >= noptions) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  if (i == def) {
    return;
  }
  if (xmlTextWriterWriteAttribute(wri, CastCxp(key), CastCxp(options[i])) <
   0) {
    throw err_write();
  }
}

void
ew::XmlWriter::write_attribute_filename(const char *val, const char *key)
{
  if (val[0] == 0) {
    return;
  }
  int lv = ew::File::pathlen(val);
  int lf = ew::File::pathlen(file);
  if (lv == 0 || lf == 0) {
    ew::Debug::instance().get_active(ClassName) &&
     ew::Debug::instance().dprintf("%s::%s bad paths val=\"%s\" file=\"%s\"",
     ClassName, "write_attribute_filename", val, file);
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  if (lv == lf && std::strncmp(val, file, lv) == 0) {
    val += lv;
  }
  if (xmlTextWriterWriteAttribute(wri, CastCxp(key), CastCxp(val)) < 0) {
    throw err_write();
  }
}

void
ew::XmlWriter::write_int(int val)
{
  char buf[ew::String::FORMAT_INT_LEN];
  ew::String::format_int(buf, ew::String::FORMAT_INT_LEN, 0, val);
  write_string(buf);
}

void
ew::XmlWriter::write_ints(const int *dd, int n, int stride)
{
  if (stride > 0) {
    write_string("\n");
  }
  for (int i = 0; i < n; i += 1) {
    write_int(dd[i]);
    if (i < n - 1) {
      if (stride > 0 && i % stride == stride - 1) {
        write_string("\n");
      } else {
        write_string(" ");
      }
    }
  }
  if (stride > 0) {
    write_string("\n");
  }
}

void
ew::XmlWriter::write_double(double val)
{
  char buf[ew::String::FORMAT_DOUBLE_LEN];
  ew::String::format_double(buf, ew::String::FORMAT_DOUBLE_LEN, 0, val);
  write_string(buf);
}

void
ew::XmlWriter::write_doubles(const double *dd, int n, int stride)
{
  if (stride > 0) {
    write_string("\n");
  }
  for (int i = 0; i < n; i += 1) {
    write_double(dd[i]);
    if (i < n - 1) {
      if (stride > 0 && i % stride == stride - 1) {
        write_string("\n");
      } else {
        write_string(" ");
      }
    }
  }
  if (stride > 0) {
    write_string("\n");
  }
}

void
ew::XmlWriter::write_transform3(const ew::Transform3 *tr)
{
  char buf[ew::Transform3::FORMAT_LEN];
  tr->format(buf, ew::Transform3::FORMAT_LEN, 0);
  write_string(buf);
}

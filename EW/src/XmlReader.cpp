// wdkg 2009-2010

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlversion.h>
#include <libxml/xmlwriter.h>
#include "ew/Debug.h"
#include "ew/ErrorLogic.h"
#include "ew/String.h"
#include "ew/ErrorIO.h"
#include "ew/String.h"
#include "ew/Transform3.h"
#include "File.h"
#include "XmlAutoDocPtr.h"
#include "XmlReader.h"

namespace {
  const char ClassName[] = "ew::XmlReader";

  inline const char *
  CastCcp(const xmlChar *p)
  {
    return reinterpret_cast<const char *>(p);
  }

  inline const xmlChar *
  CastCxp(const char *p)
  {
    return reinterpret_cast<const xmlChar *>(p);
  }

  inline void *
  CastVp(const char *p)
  {
    return static_cast<void *>(const_cast<char *>(p));
  }

// I'm not sure if this is necessary, I've never triggered it.
// Use it anyway, to ensure libxml2 never writes to stdout.

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
      std::vsnprintf(buf, ew::XmlReader::BUFSZ, msg, vargs);
    }
    va_end(vargs);
  }

}

ew::XmlReader::XmlReader(const char *f, const char *ty, const char *version) :
// root uninitialized
// children uninitialized
// err_buf uninitialized
 file(f),
 type(ty)
{
  err_buf[0] = 0;
  xmlSetGenericErrorFunc(err_buf, XmlErrorHandler);
// XML_PARSE_COMPACT is undefined in early versions of libxml2.
  int opts = XML_PARSE_NONET | XML_PARSE_NOERROR | XML_PARSE_NOWARNING;
#if LIBXML_VERSION >= 20621
  opts |= XML_PARSE_COMPACT;
#endif
  doc.read_file(f, opts);
//XXX check file existence -> separate error
  if (doc == 0) {
    throw err_desc("the file could not be opened or"
     " does not contain valid xml");
  }
  root = xmlDocGetRootElement(doc);
  if (root == 0 || std::strcmp(CastCcp(root->name), ty)) {
    throw err_desc_line("wrong incorrect document type",
     root->line);
  }
  std::string ver;
  ew::XmlReader::read_attribute(&ver, "version", root);
  if (ver.compare(version) != 0) {
    throw err_desc_line("wrong version", root->line);
  }
  children = root->xmlChildrenNode;
}

ew::ErrorIO
ew::XmlReader::err_desc(const char *desc)
{
  if (err_buf[0] != 0) {
    return ew::ErrorIO(ew::String::ssprintf(
     "The %s file %s could not be loaded (%s, %s).", type, file, desc,
     err_buf));
  } else {
    return ew::ErrorIO(ew::String::ssprintf(
     "The %s file %s could not be loaded (%s).", type, file, desc));
  }
}

ew::ErrorIO
ew::XmlReader::err_desc_line(const char *desc, int line)
{
  return ew::ErrorIO(ew::String::ssprintf(
   "The %s file %s could not be loaded"
   " (invalid format, %s in line %d).",
   type, file, desc, line));
}

ew::ErrorIO
ew::XmlReader::err_key_line(const char *key, int line)
{
  return ew::ErrorIO(ew::String::ssprintf(
   "The %s file %s could not be loaded"
   " (invalid format, bad %s value in line %d).",
   type, file, key, line));
}

void
ew::XmlReader::read_attribute(std::string *val, const char *key, xmlNode *node)
{
  const char *s = CastCcp(xmlGetProp(node, CastCxp(key)));
  if (s != 0) {
    *val = s;
    xmlFree(CastVp(s));
  } else {
    val->clear();
  }
}

void
ew::XmlReader::read_attribute_n(int *n, const char *key, xmlNode *node,
 int def)
{
  const char *s = CastCcp(xmlGetProp(node, CastCxp(key)));
  if (s != 0) {
    const char *s1;
    bool r = (ew::String::scan_int(n, &s1, s) && *s1 == 0);
    xmlFree(CastVp(s));
    if (!r) {
      throw err_key_line(key, node->line);
    }
  } else {
    *n = def;
  }
}

void
ew::XmlReader::read_attribute_list(int *val, const char *key,
 const char *const *options, xmlNode *node, int def)
{
  const char *s = CastCcp(xmlGetProp(node, CastCxp(key)));
  if (s != 0) {
    int i = 0;
    while (options[i] && std::strcmp(s, options[i]) != 0) {
      i += 1;
    }
    xmlFree(CastVp(s));
    if (options[i] == 0) {
      throw err_key_line(key, node->line);
    }
    *val = i;
  } else {
    *val = def;
  }
}

void
ew::XmlReader::read_attribute_filename(std::string *val, const char *key,
 xmlNode *node)
{
  const char *s = CastCcp(xmlGetProp(node, CastCxp(key)));
  val->clear();
  if (s != 0) {
    if (*s != 0) {
      int ls = ew::File::pathlen(s);
      if (ls == 0) {
        int lf = ew::File::pathlen(file);
        if (lf == 0) {
          ew::Debug::instance().get_active(ClassName) &&
           ew::Debug::instance().dprintf(
           "%s::%s bad paths s=\"%s\" file=\"%s\"",
           ClassName, "read_attribute_filename", s, file);
          throw ew::ErrorLogic(__FILE__, __LINE__);
        }
        *val = file;
        val->resize(lf);
        *val += s;
      } else {
        *val = s;
      }
    }
    xmlFree(CastVp(s));
  }
}

void
ew::XmlReader::read_ints(int *dd, int l, xmlNode *node)
{
  const char *s = CastCcp(xmlNodeGetContent(node));
  if (s != 0) {
    const char *s1 = s;
    int j = 0;
    while (j < l) {
      if (ew::String::scan_ws(&s1, s1) <= 0 && j != 0) {
        break;
      }
      int v;
      if (!ew::String::scan_int(&v, &s1, s1)) {
        break;
      }
      dd[j] = v;
      j += 1;
    }
    ew::String::scan_ws(&s1, s1);
    if (*s1 != 0) {
      j -= 1;
    }
    xmlFree(CastVp(s));
    if (j == l) {
      return;
    }
  }
  throw err_key_line(CastCcp(node->name), node->line);
}

void
ew::XmlReader::read_doubles(double *dd, int l, xmlNode *node)
{
  const char *s = CastCcp(xmlNodeGetContent(node));
  if (s != 0) {
    const char *s1 = s;
    int j = 0;
    while (j < l) {
      if (ew::String::scan_ws(&s1, s1) <= 0 && j != 0) {
        break;
      }
      double v;
      if (!ew::String::scan_finite_double(&v, &s1, s1)) {
        break;
      }
      dd[j] = v;
      j += 1;
    }
    ew::String::scan_ws(&s1, s1);
    if (*s1 != 0) {
      j -= 1;
    }
    xmlFree(CastVp(s));
    if (j == l) {
      return;
    }
  }
  throw err_key_line(CastCcp(node->name), node->line);
}

void
ew::XmlReader::read_transform3(ew::Transform3 *tr, xmlNode *node)
{
  const char *s = CastCcp(xmlNodeGetContent(node));
  if (s != 0) {
    const char *s1 = s;
    ew::String::scan_ws(&s1, s1);
    ew::Transform3 tr1;
    bool r = tr1.scan(&s1, s1);
    ew::String::scan_ws(&s1, s1);
    char c = *s1;
    xmlFree(CastVp(s));
    if (r && c == 0) {
      *tr = tr1;
      return;
    }
  }
  throw err_key_line(CastCcp(node->name), node->line);
}

void
ew::XmlReader::read_list(int *val, const char *const *choices, xmlNode *node)
{
  const char *s = CastCcp(xmlNodeGetContent(node));
  if (s != 0) {
    const char *s1 = s;
    ew::String::scan_ws(&s1, s1);
    int i = 0;
    while (choices[i]) {
      int l = std::strlen(choices[i]);
      if (std::strncmp(s1, choices[i], l) == 0) {
        s1 += l;
        break;
      }
      i += 1;
    }
    ew::String::scan_ws(&s1, s1);
    char c = *s1;
    xmlFree(CastVp(s));
    if (choices[i] != 0 && c == 0) {
      *val = i;
      return;
    }
  }
  throw err_key_line(CastCcp(node->name), node->line);
}

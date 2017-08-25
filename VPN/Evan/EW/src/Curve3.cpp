// wdkg 2008-2010
//XXX use stdio instead of iostream

#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "ew/Debug.h"
#include "ew/Time.h"
#include "ew/Curve3.h"
#include "ew/ErrorIO.h"
#include "ew/String.h"
#include "AutoArray.h"

namespace {
  const char ClassName[] = "ew::Curve3";

// This clears the vector and frees the reserve capacity.
  template<typename T>
  inline void
  VectorReset(std::vector<T> &v)
  {
    std::vector<T> v1;
    v.swap(v1);
  }
}

/// This frees any current data and sets the object to the initial
/// state.

void
ew::Curve3::reset()
{
  VectorReset(points);
  VectorReset(edges);
}

/// This reads the surface contained in the @a file.
/// The following surface formats are supported:
/// - obj format
///   Only these types of record are currently supported:
///   - v
///   - l with non-negative arguments
/// .
/// @param filename the name of the file to read
/// @exception std::bad_alloc
/// @exception ew::ErrorIO

void
ew::Curve3::read_file(const char *filename)
{
  double t0;
  const ew::Debug &dbg = ew::Debug::instance();
  bool dbg_on = dbg.get_active(ClassName);
  if (dbg_on) {
    dbg.dprintf("%s::read_file(\"%s\") {", ClassName, filename);
    t0 = ew::Time::elapsed();
  }
//XXX on windows need to convert to utf16
// const unsigned short* utf16=filepath.utf16(); (if using a QString)
// then inp(utf16, ...)
  std::ifstream inp(filename, (std::ios::binary | std::ios::in));
  if (!inp.is_open()) {
//XXX separate error for no such file
    throw ew::ErrorIO(ew::String::ssprintf(
     "The curve file %s could not be loaded (the file could not be opened).",
     filename));
  }
  inp.seekg(0, std::ios::end);
  int l = static_cast<int>(inp.tellg());
  inp.seekg(0, std::ios::beg);
  ew::AutoArray<char> buf(l + 1);
  dbg_on && dbg.dprintf("%s::read_file   opened/allocated", ClassName);
  inp.read(buf, l);
  buf[l] = 0;
  inp.close();
  if (!inp.good()) {
    throw ew::ErrorIO(ew::String::ssprintf(
     "The curve file %s could not be loaded"
     " (the content could not be read)",
     filename));
  }
  dbg_on && dbg.dprintf("%s::read_file   read", ClassName);
  const char *s = buf;
// Using deque here had no overhead compared to manual blocked new[].
  std::deque<float> vv;
  std::deque<int> ee;
  int ln = 0;
  int minvref = 1;
  int maxvref = 0;
  while (1) {
    ln += 1;
    const char *s0;
    ew::String::scan_ws_not_nl(&s0, s);
    ew::String::scan_not_ws(&s, s0);
    if (s - s0 == 1 && *s0 == 'v') {
      float vals[3];
      int nval = 0;
      while (1) {
        ew::String::scan_ws_not_nl(&s, s);
        if (*s == '\n') {
          s += 1;
          break;
        }
        if (*s == 0) {
          break;
        }
        float val;
        if (!ew::String::scan_finite_float(&val, &s, s)) {
          throw ew::ErrorIO(ew::String::ssprintf(
           "The curve obj file %s could not be loaded"
           " (invalid format, bad floating point value in line %d).",
           filename, ln));
        }
        if (nval < 3) {
          vals[nval] = val;
        }
        nval += 1;
      }
      if (nval != 3) {
        throw ew::ErrorIO(ew::String::ssprintf(
         "The curve obj file %s could not be loaded"
         " (invalid format,"
         " wrong number of floating point values in line %d).",
         filename, ln));
      }
      vv.push_back(vals[0]);
      vv.push_back(vals[1]);
      vv.push_back(vals[2]);
    } else if (s - s0 == 1 && *s0 == 'e') {
      int vals[1];
      int nval = 0;
      while (1) {
        ew::String::scan_ws_not_nl(&s, s);
        if (*s == '\n') {
          s += 1;
          break;
        }
        if (*s == 0) {
          break;
        }
        int val;
        if (!ew::String::scan_int(&val, &s, s)) {
          throw ew::ErrorIO(ew::String::ssprintf(
           "The curve obj file %s could not be loaded"
           " (invalid format, bad vertex number in line %d).",
           filename, ln));
        }
        int tex = -1;
        if (*s == '/') {
          s += 1;
          if (ew::String::is_nws(*s) && *s != '/'
           && !ew::String::scan_int(&tex, &s, s)) {
            throw ew::ErrorIO(ew::String::ssprintf(
             "The curve obj file %s could not be loaded"
             " (invalid format, bad normal number in line %d).",
             filename, ln));
          }
        }
        if (val < minvref) {
          minvref = val;
        }
        if (val > maxvref) {
          maxvref = val;
        }
        if (nval < 1) {
          vals[nval] = val;
        } else {
          ee.push_back(vals[0] - 1);
          ee.push_back(val - 1);
        }
        nval += 1;
      }
    } else {
// Ignore unknown lines.
      if (!ew::String::scan_line(&s, s)) {
        break;
      }
    }
  }
  int nvp = vv.size();
  int ner = ee.size();
  if (minvref < 1 || maxvref > nvp / 3) {
    throw ew::ErrorIO(ew::String::ssprintf(
     "The curve obj file %s could not be loaded"
     " (invalid format, vertex references are out of range).",
     filename));
  }
  if (ner == 0) {
    throw ew::ErrorIO(ew::String::ssprintf(
     "The curve obj file %s could not be loaded"
     " (invalid format, there are no edges).",
     filename));
  }
  dbg_on && dbg.dprintf("%s::read_file   scanned", ClassName);
// Convert deque to vector.
  std::vector<float> points1;
  std::vector<int> edges1;
  points1.reserve(nvp);
  edges1.reserve(ner);
  for (int i = 0; i < nvp; i += 1) {
    points1.push_back(vv[i]);
  }
  for (int i = 0; i < ner; i += 1) {
    edges1.push_back(ee[i]);
  }
  reset();
  points.swap(points1);
  edges.swap(edges1);
  if (dbg_on) {
    double t1 = ew::Time::elapsed();
    dbg.dprintf("%s::read_file   repacked", ClassName);
    dbg.dprintf("%s::read_file   elapsed=%.6f", ClassName, t1 - t0);
    dbg.dprintf("%s::read_file }", ClassName);
  }
}

/// This reads a curve as a list of points.
/// @param coords the vector of coordinates
/// @exception std::bad_alloc

void
ew::Curve3::read_points(const std::vector<double> &coords)
{
  for (int i = 0; i < static_cast<int>(coords.size()); i += 1) {
    points[i] = static_cast<float>(coords[i]);
  }
  for (int i = 0; i + 1 < static_cast<int>(coords.size()) / 3; i += 1) {
    edges[2 * i] = i + 1;			//YS (2 July 2017): indexing should start from 1 not 0
    edges[2 * i + 1] = i + 2;
  }
}

/// This writes the surface to the @a file in obj format.
/// @param filename the name of the file to write
/// @exception ew::ErrorIO

void
ew::Curve3::write_file(const char *filename) const
{
//XXX autoFilePtr
  std::FILE *fd = std::fopen(filename, "wb");
  if (fd == 0) {
    throw ew::ErrorIO(ew::String::ssprintf(
     "The curve file %s could not be saved (the file could not be opened).",
     filename));
  }
  for (int i = 0; i < static_cast<int>(points.size()); i += 3) {
    const int l = 3 * ew::String::FORMAT_FLOAT_LEN + 5;
    char buf[l];
    int o = 0;
    int ol;
    buf[0] = 'v';
    buf[1] = ' ';
    o += 2;
    for (int j = 0; j < 3; j += 1) {
      ew::String::format_float(buf + o, l - o, &ol, points[i + j]);
      o += ol;
      buf[ol] = ' ';
      o += 1;
    }
    buf[o - 1] = '\n';
    if (std::fwrite(buf, 1, o, fd) != static_cast<std::size_t>(o)) {
      std::fclose(fd);
      throw ew::ErrorIO(ew::String::ssprintf(
       "The curve file %s could not be saved (the file could not be written).",
       filename));
    }
  }
  for (int i = 0; i < static_cast<int>(edges.size()); i += 2) {
    const int l = 2 * ew::String::FORMAT_INT_LEN + 5;
    char buf[l];
    int o = 0;
    int ol;
    buf[0] = 'e';
    buf[1] = ' ';
    o += 2;
    for (int j = 0; j < 2; j += 1) {
      ew::String::format_int(buf + o, l - o, &ol, edges[i + j]);
      o += ol;
      buf[ol] = ' ';
      o += 1;
    }
    buf[o - 1] = '\n';
    if (std::fwrite(buf, 1, o, fd) != static_cast<std::size_t>(o)) {
      std::fclose(fd);
      throw ew::ErrorIO(ew::String::ssprintf(
       "The curve file %s could not be saved (the file could not be written).",
       filename));
    }
  }
  if (std::fclose(fd) != 0) {
    throw ew::ErrorIO(ew::String::ssprintf(
     "The curve file %s could not be saved (the file could not be closed).",
     filename));
  }
  return;
}

void ew::Curve3::write_file_txt(const char *filename) const
{
	std::ofstream fd;
	fd.open(filename);
	if(!fd.is_open())
	{
		throw ew::ErrorIO(ew::String::ssprintf(
		     "The curve file %s could not be saved (the file could not be opened).",
		     filename));
	}
	for (int i=0; i<static_cast<int>(points.size()); i+=3)
		fd << "v "<< points[i] << " " << points[i+1] << " " << points[i+2] << std::endl;
	for (int i=0; i<static_cast<int>(edges.size()); i+=2)
			fd << "e "<< edges[i] << " " << edges[i+1] << std::endl;
	fd.close();
}

/// Compares this curve with another, member by member.
/// @param a the other curve

bool
ew::Curve3::operator==(const ew::Curve3 &a) const
{
  return points == a.points && edges == a.edges;
}

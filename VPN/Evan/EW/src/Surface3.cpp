// wdkg 2008-2010
//XXX use stdio instead of iostream

#include <cstdio>
#include <cstring>
#include <deque>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "ew/Debug.h"
#include "ew/Time.h"
#include "ew/ErrorIO.h"
#include "ew/String.h"
#include "ew/Surface3.h"
#include "AutoArray.h"

namespace {
  const char ClassName[] = "ew::Surface3";

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
ew::Surface3::reset()
{
  VectorReset(points);
  VectorReset(faces);
}

/// This reads the surface contained in the @a file.
/// The following surface formats are supported:
/// - obj format, for example @ref tetrahedron.obj.
///   Only these types of record are currently supported:
///   - v
///   - f with non-negative arguments
///   .
///   Normals and texture mapping are currently ignored.
/// .
/// @param filename
/// The name of the file to read.
/// @exception std::bad_alloc
/// @exception ew::ErrorIO

void
ew::Surface3::read_file(const char *filename)
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
//XXX backslash line continuation
//XXX use fopen instead
//XXX fn to read data (adapt to no zero termination)
//XXX not an obj sur return:
//XXX other formats ?stl ply 3dxml x3d ...
//XXX gzipped
//  anything other than
//   #
//   empty
//   end
//   parm
//   surf
  std::ifstream inp(filename, (std::ios::binary | std::ios::in));
  if (!inp.is_open()) {
//XXX separate error for no such file
    throw ew::ErrorIO(ew::String::ssprintf(
     "The surface file %s could not be loaded"
     " (the file could not be opened).",
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
     "The surface file %s could not be loaded"
     " (the content could not be read)",
     filename));
  }
  dbg_on && dbg.dprintf("%s::read_file   read", ClassName);
  const char *s = buf;
// Using deque here had no overhead compared to manual blocked new[].
  std::deque<float> vv;
  std::deque<int> ff;
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
        std::stringstream valueStrStream(std::string(s,32)); //32 digit number maximum "probably an overkill"
        valueStrStream >> val;
        s+=valueStrStream.tellg();
        /*if (!ew::String::scan_finite_float(&val, &s, s)) {			//Bug with reading scientific notation (xxxe-xx)
          throw ew::ErrorIO(ew::String::ssprintf(
           "The surface obj file %s could not be loaded"
           " (invalid format, bad floating point value in line %d).",
           filename, ln));
        }*/
        if (nval < 3) {
          vals[nval] = val;
        }
        nval += 1;
      }
      if (nval != 3) {
        throw ew::ErrorIO(ew::String::ssprintf(
         "The surface obj file %s could not be loaded"
         " (invalid format,"
         " wrong number of floating point values in line %d).",
         filename, ln));
      }
      vv.push_back(vals[0]);
      vv.push_back(vals[1]);
      vv.push_back(vals[2]);
    } else if (s - s0 == 1 && *s0 == 'f') {
      int vals[2];
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
           "The surface obj file %s could not be loaded"
           " (invalid format, bad vertex number in line %d).",
           filename, ln));
        }
        int norm = -1;
        int tex = -1;
        if (*s == '/') {
          s += 1;
          if (ew::String::is_nws(*s) && *s != '/'
           && !ew::String::scan_int(&norm, &s, s)) {
            throw ew::ErrorIO(ew::String::ssprintf(
             "The surface obj file %s could not be loaded"
             " (invalid format, bad normal number in line %d).",
             filename, ln));
          }
          if (*s == '/') {
            s += 1;
            if (ew::String::is_nws(*s) && !ew::String::scan_int(&tex, &s, s)) {
              throw ew::ErrorIO(ew::String::ssprintf(
               "The surface obj file %s could not be loaded"
               " (invalid format, bad texture map number in line %d).",
               filename, ln));
            }
          }
        }
        if (val < minvref) {
          minvref = val;
        }
        if (val > maxvref) {
          maxvref = val;
        }
        if (nval < 2) {
          vals[nval] = val;
        } else {
          ff.push_back(vals[0] - 1);
          ff.push_back(vals[1] - 1);
          ff.push_back(val - 1);
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
  int nfr = ff.size();
  if (minvref < 1 || maxvref > nvp / 3) {
    throw ew::ErrorIO(ew::String::ssprintf(
     "The surface obj file %s could not be loaded"
     " (invalid format, vertex references are out of range).",
     filename));
  }
  if (nfr == 0) {
    throw ew::ErrorIO(ew::String::ssprintf(
     "The surface obj file %s could not be loaded"
     " (invalid format, there are no faces).",
     filename));
  }
  dbg_on && dbg.dprintf("%s::read_file   scanned", ClassName);
// Convert deque to vector.
  std::vector<float> points1;
  std::vector<int> faces1;
  points1.reserve(nvp);
  faces1.reserve(nfr);
  for (int i = 0; i < nvp; i += 1) {
    points1.push_back(vv[i]);
  }
  for (int i = 0; i < nfr; i += 1) {
    faces1.push_back(ff[i]);
  }
  reset();
  points.swap(points1);
  faces.swap(faces1);
  if (dbg_on) {
    double t1 = ew::Time::elapsed();
    dbg.dprintf("%s::read_file   repacked", ClassName);
    dbg.dprintf("%s::read_file   elapsed=%.6f", ClassName, t1 - t0);
    dbg.dprintf("%s::read_file }", ClassName);
  }
}

/// Compares this surface with another, member by member.
/// @param a the other surface

bool
ew::Surface3::operator==(const ew::Surface3 &a) const
{
  return points == a.points && faces == a.faces;
}

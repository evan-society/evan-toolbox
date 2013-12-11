#ifndef EW_SURFACE3H_INCLUDED
#define EW_SURFACE3H_INCLUDED

// wdkg 2008-2010

//XXX auto try xxx.gz first, any error -> xxx
//XXX ? if filename to write ends in gz, compress automatically, otherwise not
//XXX support texture maps, normals
//XXX more specific error info (line)
//XXX mmap (check ws terminated, o/w copy and add z) or incremental read
// to use readahead (profile)
//XXX check not large file
//XXX more specific error classes (no file, io error, format error)
//XXX read compressed
//XXX ? mmap cached format, check for *.cache
//XXX other formats (ply), more general obj support, cache format
//XXX ?auto-detect format, ?auto cache support
//XXX other formats (ply), more general obj support, cache format
//XXX ? uniquify vertices
//XXX silhouette, flat, wireframe, text map, normals, ...
//XXX write surface
//XXX obj: relative index, vn, vt, usemtl (or mtl file)
// more specific error info (line)
// check not large file

#include <vector>

namespace ew {
  class Surface3 {
  public:
    std::vector<float> points;
    std::vector<int> faces;
    void reset();
    void read_file(const char *file);
    inline void swap(ew::Surface3 &s);
    bool operator==(const ew::Surface3 &a) const;
    inline bool operator!=(const ew::Surface3 &a) const;
  };
}

/// @class ew::Surface3
/// @headerfile ew/Surface3.h "ew/Surface3.h"
/// @brief Triangulated Surface
///
/// ew::Surface3 encapsulates data for triangulated surfaces in
/// @f$\mathbb{R}^3@f$.
///
/// ew::Surface3 is a data structure that supports default construction, copy
/// construction, assignment and equality comparison.
///
/// The data consists of a list of points and a list of faces.
/// If a point is shared by several faces, the surface normal will be avaraged
/// while rendering as a smooth surface.
/// It this is not the intent, the point should be duplicated as several
/// entries in the list of points, all with the same coordinates.

/// @var std::vector<float> ew::Surface3::points
/// This vector has size 3 times the number of points.
/// Each consecutive triple defines a point.

/// @var std::vector<int> ew::Surface3::faces
/// This vector has size 3 times the number of faces
/// The elements are indices of the points list, and are in the range
/// [0, number of points).
/// Each consequtive triple defines a face.

/// Swap data between 2 surfaces without copying.
/// @param that
///   The surface to swap with @c this.

inline void
ew::Surface3::swap(ew::Surface3 &that)
{
  points.swap(that.points);
  faces.swap(that.faces);
}

/// Compares this surface with another, member by member.
/// @param a the other surface

inline bool
ew::Surface3::operator!=(const ew::Surface3 &a) const
{
  return !operator==(a);
}

#endif

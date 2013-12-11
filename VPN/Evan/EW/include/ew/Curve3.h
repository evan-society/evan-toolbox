#ifndef EW_CURVE3_H_INCLUDED
#define EW_CURVE3_H_INCLUDED

// wdkg 2008-2010

//XXX auto try xxx.gz first, any error -> xxx
//XXX ? if filename to write ends in gz, compress automatically, otherwise not
//XXX more specific error info (line)
//XXX check not large file
//XXX more specific error classes (no file, io error, format error)
//XXX read compressed
//XXX ? mmap cached format, check for *.cache
//XXX other formats (ply), more general obj support, cache format
//XXX ?auto-detect format, ?auto cache support
//XXX ? uniquify vertices
//XXX write curve
//XXX obj: relative index

#include <vector>

namespace ew {
  class Curve3 {
  public:
    std::vector<float> points;
    std::vector<int> edges;
    void reset();
    void read_file(const char *file);
    void read_points(const std::vector<double> &coords);
    void write_file(const char *file) const;
    inline void swap(ew::Curve3 &s);
    bool operator==(const ew::Curve3 &a) const;
    inline bool operator!=(const ew::Curve3 &a) const;
  };
}

/// @class ew::Curve3
/// @headerfile ew/Curve3.h "ew/Curve3.h"
/// @brief Piecewise-linear Curve
///
/// ew::Curve3 encapsulates data for piecewise-linear curves in
/// @f$\mathbb{R}^3@f$.
///
/// The data consists of a vector of points and a vector of edges.
///
/// ew::Curve3 is a data structure that supports default construction, copy
/// construction and assignment.

/// @var ew::Curve3::points
/// This vector has size 3 times the number of points.
/// Each consecutive triple defines a point.

/// @var ew::Curve3::edges
/// This vector has size 2 times the number of edges.
/// The elements are indices of the points list, and are in the range
/// [0, number of points).
/// Each consecutive pair defines an edge.

/// Swap data between 2 curves without copying.
/// @param s the surface to swap @c this with

inline void
ew::Curve3::swap(ew::Curve3 &s)
{
  points.swap(s.points);
  edges.swap(s.edges);
}

/// Compares this curve with another, member by member.
/// @param a the other curve

inline bool
ew::Curve3::operator!=(const ew::Curve3 &a) const
{
  return !operator==(a);
}

#endif

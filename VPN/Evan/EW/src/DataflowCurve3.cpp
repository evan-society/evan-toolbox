// wdkg 2008-2010

#include <memory>
#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowCurve3.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "Geom3.h"
#include "Index3.h"

namespace {
  const char ClassName[] = "ew::DataflowCurve3";

  template<typename T>
  inline int
  VectorSize(const std::vector<T> &v)
  {
    return static_cast<int>(v.size());
  }

// This clears the vector and frees the reserve capacity.
  template<typename T>
  inline void
  VectorReset(std::vector<T> &v)
  {
    std::vector<T> v1;
    v.swap(v1);
  }
}

ew::DataflowCurve3::DataflowCurve3(ew::DataflowNetwork *i_network,
 int ndeps, const char *dbg_final_name,
 const char *const *const i_dbg_dependency_names) :
 ew::DataflowNode(i_network, ndeps, dbg_final_name, i_dbg_dependency_names),
 dlist(0),
 dlist_cycle(0),
 dlist_refs(0),
 dbg(ClassName, ew::DataflowNode::dbg),
 update_cycle_index(0),
 update_cycle_tangents(0),
 outp_index(new ew::Index3),
 update_cycle_bbox(0)
{
	outp_points = NULL;
	outp_num_points = 0;

	outp_edges = NULL;
	outp_num_edges = 0;
}

// This is to keep auto_ptr happy.

ew::DataflowCurve3::~DataflowCurve3()
{
}

void
ew::DataflowCurve3::update_bbox() const
{
  dbg.on && dbg.dprintf("%s::%s UPDATE", dbg.fn, "update_bbox");
// This will count unused points.
  outp_bbox = ew::Bbox3::empty_box;
  outp_bbox.set_to_points(outp_points, outp_num_points);
}

// This obviously is not intended for curves with self-intersection.

void
ew::DataflowCurve3::update_tangents() const
{
  dbg.on && dbg.dprintf("%s::%s UPDATE", dbg.fn, "update_tangents");
// No harm using a vector for tangents because we need initialization.
  if (VectorSize(outp_tangents) == outp_num_points * 2) {
    for (int i = 0; i < outp_num_points * 2; i += 1) {
      outp_tangents[i] = 0;
    }
  } else {
    outp_tangents.resize(0);
    outp_tangents.resize(outp_num_points * 2, 0.0);
  }
//XXX use circle fitting
//XXX use Geom3, better == 0 cutoff, don't include bad tangents in
// average
  for (int i = 0; i < outp_num_edges; i += 1) {
    int a = outp_edges[2 * i];
    int b = outp_edges[2 * i + 1];
    double ax = outp_points[2 * a];
    double ay = outp_points[2 * a + 1];
    double az = outp_points[2 * a + 2];
    double bx = outp_points[2 * b];
    double by = outp_points[2 * b + 1];
    double bz = outp_points[2 * b + 2];
    double tx = bx - ax;
    double ty = by - ay;
    double tz = bz - az;
    double tn = std::sqrt (tx * tx + ty * ty + tz * tz);
    if (tn != 0.0) {
      tx /= tn;
      ty /= tn;
      tz /= tn;
    } else {
      tx = 1.0;
      ty = tz = 0.0;
    }
    outp_tangents[3 * a] += tx;
    outp_tangents[3 * a + 1] += ty;
    outp_tangents[3 * a + 2] += tz;
    outp_tangents[3 * b] += tx;
    outp_tangents[3 * b + 1] += ty;
    outp_tangents[3 * b + 2] += tz;
  }
  for (int i = 0; i < outp_num_points; i += 1) {
    double tx = outp_tangents[3 * i];
    double ty = outp_tangents[3 * i + 1];
    double tz = outp_tangents[3 * i + 2];
    double tn = std::sqrt (tx * tx + ty * ty + tz * tz);
    if (tn != 0.0) {
      tx /= tn;
      ty /= tn;
      tz /= tn;
    } else {
      tx = 1.0;
      ty = tz = 0.0;
    }
    outp_tangents[3 * i] = tx;
    outp_tangents[3 * i + 1] = ty;
    outp_tangents[3 * i + 2] = tz;
  }
}

void
ew::DataflowCurve3::reset_tangents() const
{
  VectorReset(outp_tangents);
}

void
ew::DataflowCurve3::update_index() const
{
  dbg.on && dbg.dprintf("%s::%s UPDATE", dbg.fn, "update_index");
  outp_index->make_index(outp_points, outp_num_points, outp_edges, 2,
   outp_num_edges);
}

void
ew::DataflowCurve3::reset_index() const
{
  outp_index->reset();
}

//XXX ? inline
double
ew::DataflowCurve3::project(int *edge, double *coeffs,
 double *tangent, double *proj, const double *inp) const
{
  make_index();
  get_tangents();
  double d = outp_index->nearest_on_polytopes(edge, proj, coeffs, inp);
// outp_edges and outp_tangents are arrays of float.
  int a = outp_edges[2 * (*edge)];
  int b = outp_edges[2 * (*edge) + 1];
  double n[6];
  for (int i = 0; i < 3; i += 1) {
    n[i] = outp_tangents[a * 3 + i];
    n[3 + i] = outp_tangents[b * 3 + i];
  }
  ew::Geom3::linear_combination(tangent, coeffs[0], n, coeffs[1], n + 3);
  double nn = ew::Geom3::norm(tangent);
//XXX
  if (nn == 0) {
    ew::Geom3::set(tangent, 1.0, 0.0, 0.0);
  } else {
    ew::Geom3::linear_combination(tangent, 1.0 / nn, tangent);
  }
  return d;
}

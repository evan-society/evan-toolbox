// wdkg 2008-2010

#include <memory>
#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowSurface3.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "Geom3.h"
#include "Index3.h"

namespace {
  const char ClassName[] = "ew::DataflowSurface3";

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

ew::DataflowSurface3::DataflowSurface3(ew::DataflowNetwork *i_network,
 int ndeps, const char *dbg_final_name,
 const char *const *const i_dbg_dependency_names) :
 ew::DataflowNode(i_network, ndeps, dbg_final_name, i_dbg_dependency_names),
 dlist(0),
 dlist_cycle(0),
 dlist_refs(0),
 dbg(ClassName, ew::DataflowNode::dbg),
 update_cycle_index(0),
 update_cycle_normals(0),
 outp_index(new ew::Index3),
 update_cycle_bbox(0)
{
	outp_faces = NULL;
	outp_num_faces = 0;

	outp_points = NULL;
	outp_num_points = 0;
}

// This is to keep auto_ptr happy.

ew::DataflowSurface3::~DataflowSurface3()
{
}

void
ew::DataflowSurface3::update_bbox() const
{
  dbg.on && dbg.dprintf("%s::%s UPDATE", dbg.fn, "update_bbox");
// This will count unused points.
  outp_bbox = ew::Bbox3::empty_box;
  outp_bbox.set_to_points(outp_points, outp_num_points);
}

void
ew::DataflowSurface3::update_normals() const
{
  dbg.on && dbg.dprintf("%s::%s UPDATE", dbg.fn, "update_normals");
// No harm using a vector for normals because we need initialization.
  if (VectorSize(outp_normals) == outp_num_points * 3) {
    for (int i = 0; i < outp_num_points * 3; i += 1) {
      outp_normals[i] = 0;
    }
  } else {
    outp_normals.resize(0);
    outp_normals.resize(outp_num_points * 3, 0.0);
  }
// Weight face normals by subtended angle.
// This makes the result invariant under subdivision.
//XXX use Geom3, better == 0 cutoff, don't include bad normals in
// average, weight average by angle subtended
  for (int i = 0; i < outp_num_faces; i += 1) {
    int a = outp_faces[3 * i];
    int b = outp_faces[3 * i + 1];
    int c = outp_faces[3 * i + 2];
    double ax = outp_points[3 * a];
    double ay = outp_points[3 * a + 1];
    double az = outp_points[3 * a + 2];
    double bx = outp_points[3 * b];
    double by = outp_points[3 * b + 1];
    double bz = outp_points[3 * b + 2];
    double cx = outp_points[3 * c];
    double cy = outp_points[3 * c + 1];
    double cz = outp_points[3 * c + 2];
    double nx = (by - ay) * (cz - az) - (bz - az) * (cy - ay);
    double ny = (bz - az) * (cx - ax) - (bx - ax) * (cz - az);
    double nz = (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
    double nn = std::sqrt (nx * nx + ny * ny + nz * nz);
    if (nn != 0.0) {
      nx /= nn;
      ny /= nn;
      nz /= nn;
    } else {
      nx = 1.0;
      ny = nz = 0.0;
    }
    outp_normals[3 * a] += nx;
    outp_normals[3 * a + 1] += ny;
    outp_normals[3 * a + 2] += nz;
    outp_normals[3 * b] += nx;
    outp_normals[3 * b + 1] += ny;
    outp_normals[3 * b + 2] += nz;
    outp_normals[3 * c] += nx;
    outp_normals[3 * c + 1] += ny;
    outp_normals[3 * c + 2] += nz;
  }
  for (int i = 0; i < outp_num_points; i += 1) {
    double nx = outp_normals[3 * i];
    double ny = outp_normals[3 * i + 1];
    double nz = outp_normals[3 * i + 2];
    double nn = std::sqrt (nx * nx + ny * ny + nz * nz);
    if (nn != 0.0) {
      nx /= nn;
      ny /= nn;
      nz /= nn;
    } else {
      nx = 1.0;
      ny = nz = 0.0;
    }
    outp_normals[3 * i] = nx;
    outp_normals[3 * i + 1] = ny;
    outp_normals[3 * i + 2] = nz;
  }
}

void
ew::DataflowSurface3::reset_normals() const
{
  VectorReset(outp_normals);
}

void
ew::DataflowSurface3::update_index() const
{
  dbg.on && dbg.dprintf("%s::%s UPDATE", dbg.fn, "update_index");
  outp_index->make_index(outp_points, outp_num_points, outp_faces, 3,
   outp_num_faces);
}

void
ew::DataflowSurface3::reset_index() const
{
  outp_index->reset();
}

//XXX ? inline
double
ew::DataflowSurface3::project(int *face, double *coeffs,
 double *normal, double *proj, const double *inp) const
{
  make_index();
  get_normals();
  double d = outp_index->nearest_on_polytopes(face, proj, coeffs, inp);
// outp_faces and outp_normals are arrays of float.
  int a = outp_faces[3 * (*face)];
  int b = outp_faces[3 * (*face) + 1];
  int c = outp_faces[3 * (*face) + 2];
  double n[9];
  for (int i = 0; i < 3; i += 1) {
    n[i] = outp_normals[a * 3 + i];
    n[3 + i] = outp_normals[b * 3 + i];
    n[6 + i] = outp_normals[c * 3 + i];
  }
  ew::Geom3::linear_combination(normal, coeffs[0], n, coeffs[1], n + 3,
   coeffs[2], n + 6);
  double nn = ew::Geom3::norm(normal);
//XXX
  if (nn == 0) {
    ew::Geom3::set(normal, 1.0, 0.0, 0.0);
  } else {
    ew::Geom3::linear_combination(normal, 1.0 / nn, normal);
  }
  return d;
}

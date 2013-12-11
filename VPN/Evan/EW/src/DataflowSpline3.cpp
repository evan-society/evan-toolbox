// wdkg 2008-2010

#include <cstring>
#include <stdexcept>
#include <vector>
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/DataflowForm3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowSpline3.h"
#include "ew/ErrorRuntime.h"
#include "ew/Form3.h"
#include "ew/Form3Pointset.h"
#include "ew/String.h"
#include "ew/Tps3.h"

namespace {
  const char ClassName[] = "ew::DataflowSpline3";
  const char InstanceName[] = "spline3";
  enum {DEP_LHS, DEP_RHS};
  const char *const DepNames[] = {"lhs", "rhs"};

  template<typename T>
  inline T*
  VectorData(std::vector<T> &v)
  {
    if (v.empty()) {
      return 0;
    } else {
      return &(v[0]);
    }
  }

// This clears the vector and frees the reserve capacity.
  template<typename T>
  inline void
  VectorReset(std::vector<T> &v)
  {
    std::vector<T> v1;
    v.swap(v1);
  }

  template<typename T>
  inline void
  VectorInsertL(std::vector<T> &v, int n, int l, const T &val)
  {
    v.insert(v.begin() + n, l, val);
  }
}

ew::DataflowSpline3::DataflowSpline3(ew::DataflowNetwork *i_network) :
 ew::DataflowNode(i_network, 2, InstanceName, DepNames),
 dbg(ClassName, ew::DataflowNode::dbg),
 update_cycle_association(0),
 update_cycle_interaction(0),
 update_cycle_factorization(0),
 update_cycle_spline(0)
{
	outp_energy = 0.0;
	outp_f_size = 0;
	outp_is_mixed = false;
	outp_is_reduced = false;
	outp_n_lmks = 0;
	outp_nonsingular = false; //? true;
	outp_nonsingularity = 0.0;
	outp_version_association = 0UL;
	outp_version_factorization = 0UL;
	outp_version_interaction = 0UL;
	outp_version_spline = 0UL;
}

void
ew::DataflowSpline3::reset()
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "reset",
   ew::DataflowNode::change_cycle);
  ew::DataflowNode::set_dependency(DEP_LHS, 0);
  ew::DataflowNode::set_dependency(DEP_RHS, 0);
  update_cycle_association = update_cycle_interaction =
   update_cycle_factorization = update_cycle_spline = 0;
  reset_association();
  reset_interaction();
  reset_factorization();
  reset_spline();
}

/// @param f
///   A pointer to the new template form node.

void
ew::DataflowSpline3::set_template(const ew::DataflowForm3 *f)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_template",
   ew::DataflowNode::change_cycle);
  ew::DataflowNode::set_dependency(DEP_LHS, f);
}

/// @param f
///   A pointer to the new specimen form node.

void
ew::DataflowSpline3::set_specimen(const ew::DataflowForm3 *f)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_specimen",
   ew::DataflowNode::change_cycle);
  ew::DataflowNode::set_dependency(DEP_RHS, f);
}

//XXX support for fast/cached warping in 2D subspaces
//XXX  ? in dataflow_form3, have a set to linear combination method

void
ew::DataflowSpline3::update_helper() const
{
  const ew::DataflowForm3 *lhs = get_template();
  const ew::DataflowForm3 *rhs = get_specimen();
  ew::DataflowNode::outp_version = ew::DataflowNode::change_cycle;
  if (lhs != 0) {
    unsigned long v = lhs->ew::DataflowNode::get_version();
    if (ew::DataflowNode::outp_version < v) {
      ew::DataflowNode::outp_version = v;
    }
  }
  if (rhs != 0) {
    unsigned long v = rhs->ew::DataflowNode::get_version();
    if (ew::DataflowNode::outp_version < v) {
      ew::DataflowNode::outp_version = v;
    }
  }
  if (ew::DataflowNode::outp_version > ew::DataflowNode::update_cycle) {
    outp_version_association = ew::DataflowNode::change_cycle;
    if (lhs != 0) {
      unsigned long v = lhs->get_change_cycle_association();
      if (outp_version_association < v) {
        outp_version_association = v;
      }
    }
    if (rhs != 0) {
      unsigned long v = rhs->get_change_cycle_association();
      if (outp_version_association < v) {
        outp_version_association = v;
      }
    }
    outp_version_interaction = outp_version_association;
    if (lhs != 0) {
      unsigned long v = lhs->get_change_cycle_coords();
      if (outp_version_interaction < v) {
        outp_version_interaction = v;
      }
    }
    outp_version_factorization = outp_version_interaction;
    if (rhs != 0) {
      unsigned long v = rhs->get_change_cycle_relax();
      if (outp_version_factorization < v) {
        outp_version_factorization = v;
      }
    }
    outp_version_spline = outp_version_factorization;
    if (rhs != 0) {
      unsigned long v = rhs->get_change_cycle_coords();
      if (outp_version_spline < v) {
        outp_version_spline = v;
      }
    }
    if (outp_version_association > update_cycle_association) {
      update_association();
      update_cycle_association = network->get_cycle();
    }
    if (outp_n_lmks < 4) {
      ew::DataflowNode::outp_valid = false;
    } else {
      ew::DataflowNode::outp_valid = true;
    }
    if (dbg.on) {
      dbg.dprintf("%s::%s UPDATE version=%lu", dbg.fn, "update_helper",
       ew::DataflowNode::outp_version);
      dbg.dprintf("%s::%s UPDATE valid=%s", dbg.fn, "update_helper",
       ew::Debug::to_str(ew::DataflowNode::outp_valid));
    }
  }
}

// This invalidates update_helper outputs, so should only be called from
// reset().

void
ew::DataflowSpline3::reset_association() const
{
  dbg.on && dbg.dprintf("%s::%s", dbg.fn, "reset_association");
  for (int side = 0; side < 2; side += 1) {
    VectorReset(outp_spline_to_ps[side]);
    VectorReset(outp_spline_to_ps_i[side]);
    VectorReset(outp_ps_to_spline[side]);
  }
}

void
ew::DataflowSpline3::update_association() const
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "update_association");
  for (int side = 0; side < 2; side += 1) {
    outp_spline_to_ps[side].clear();
    outp_spline_to_ps_i[side].clear();
    outp_ps_to_spline[side].clear();
  }
  outp_n_lmks = 0;
  const ew::DataflowForm3 *lhs = get_template();
  const ew::DataflowForm3 *rhs = get_specimen();
  int lhs_n = 0;
  if (lhs != 0) {
    lhs_n = lhs->get_data()->pointsets.size();
  }
  int rhs_n = 0;
  if (rhs != 0) {
    rhs_n = rhs->get_data()->pointsets.size();
  }
  VectorInsertL(outp_ps_to_spline[0], 0, lhs_n, -1);
  VectorInsertL(outp_ps_to_spline[1], 0, rhs_n, -1);
  if (lhs_n != 0 && rhs_n != 0) {
    const ew::Form3PointSet *lhs_ps = &lhs->get_data()->pointsets[0];
    const ew::Form3PointSet *rhs_ps = &rhs->get_data()->pointsets[0];
    int i = 0;
    int j = 0;
    while (i < lhs_n && j < rhs_n) {
      int c = std::strcmp(lhs_ps[i].id.c_str(), rhs_ps[j].id.c_str());
      if (c < 0) {
        i += 1;
        continue;
      }
      if (c > 0) {
        j += 1;
        continue;
      }
      int nl = lhs_ps[i].n;
      int nt = lhs_ps[i].type;
// Only associate pointsets if they have the same size and type and if they
// are landmarks or semi-landmarks.
      if (nl != 0 && nl == rhs_ps[j].n && (nt == ew::Form3::TYPE_LANDMARK ||
       nt == ew::Form3::TYPE_SEMI_LANDMARK) && nt == rhs_ps[j].type) {
        outp_ps_to_spline[0][i] = outp_n_lmks;
        outp_ps_to_spline[1][j] = outp_n_lmks;
        for (int k = 0; k < nl; k += 1) {
          outp_spline_to_ps[0].push_back(i);
          outp_spline_to_ps[1].push_back(j);
          outp_spline_to_ps_i[0].push_back(k);
          outp_spline_to_ps_i[1].push_back(k);
          outp_n_lmks += 1;
        }
      }
      i += 1;
      j += 1;
    }
  }
  if (dbg.on) {
    dbg.dprintf("%s::%s UPDATE n_lmks=%d", dbg.fn, "update_association",
     outp_n_lmks);
    dbg.dprintf("%s::%s }", dbg.fn, "update_association");
  }
}

void
ew::DataflowSpline3::reset_interaction() const
{
  dbg.on && dbg.dprintf("%s::%s", dbg.fn, "reset_interaction");
  VectorReset(outp_lmks);
  VectorReset(outp_interaction);
}

void
ew::DataflowSpline3::update_interaction() const
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "update_interaction");
  outp_lmks.resize(outp_n_lmks * 3);
  outp_interaction.resize((outp_n_lmks + 4) * (outp_n_lmks + 4));
  const ew::DataflowForm3 *lhs = get_template();
  const ew::Form3PointSet *lhs_ps = &lhs->get_data()->pointsets[0];
  int i = 0;
  while (i < outp_n_lmks) {
    const ew::Form3PointSet *psi = lhs_ps + outp_spline_to_ps[0][i];
    if (psi->locations.size() == 0) {
      for (int j = 0; j < psi->n; j += 1) {
        outp_lmks[3 * (i + j) + 0] =
         outp_lmks[3 * (i + j) + 1] =
         outp_lmks[3 * (i + j) + 2] = 0.0;
      }
    } else {
      for (int j = 0; j < psi->n; j += 1) {
        outp_lmks[3 * (i + j) + 0] = psi->locations[3 * j + 0];
        outp_lmks[3 * (i + j) + 1] = psi->locations[3 * j + 1];
        outp_lmks[3 * (i + j) + 2] = psi->locations[3 * j + 2];
      }
    }
    i += psi->n;
  }
  dbg.on && dbg.dprintf("%s::%s UPDATE ew::Tps::interaction", dbg.fn,
   "update_interaction");
  ew::Tps3::interaction(VectorData(outp_interaction),
   VectorData(outp_lmks), outp_n_lmks);
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "update_interaction");
}

void
ew::DataflowSpline3::reset_factorization() const
{
  dbg.on && dbg.dprintf("%s::%s", dbg.fn, "reset_factorization");
  VectorReset(outp_relax_dims);
  VectorReset(outp_relax_params);
  VectorReset(outp_factorization);
  VectorReset(outp_pivots);
}

void
ew::DataflowSpline3::update_factorization() const
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "update_factorization");
  const ew::DataflowForm3 *rhs = get_specimen();
  const ew::Form3PointSet *rhs_ps = &rhs->get_data()->pointsets[0];
  bool has_relax_dims = false;
  int i = 0;
  while (i < outp_n_lmks) {
    const ew::Form3PointSet *psi = rhs_ps + outp_spline_to_ps[1][i];
    if (psi->relax_dims.size() != 0) {
      for (int j = 0; j < psi->n; j += 1) {
        if (psi->relax_dims[j] != 0) {
          has_relax_dims = true;
          i = outp_n_lmks;
          break;
        }
      }
    }
    i += psi->n;
  }
  bool has_relax_params = false;
  if (has_relax_dims) {
    outp_relax_dims.resize(outp_n_lmks);
    int i = 0;
    while (i < outp_n_lmks) {
      const ew::Form3PointSet *psi = rhs_ps + outp_spline_to_ps[1][i];
      if (psi->relax_dims.size() != 0) {
        for (int j = 0; j < psi->n; j += 1) {
          outp_relax_dims[i + j] = psi->relax_dims[j];
          if (psi->relax_dims[j] != 0 && psi->relax_dims[j] != 3) {
            has_relax_params = true;
          }
        }
      } else {
        for (int j = 0; j < psi->n; j += 1) {
          outp_relax_dims[i + j] = 0;
        }
      }
      i += psi->n;
    }
  } else {
    outp_relax_dims.resize(0);
  }
  ew::Tps3::algebraic_size(&outp_f_size, &outp_is_mixed, &outp_is_reduced,
   VectorData(outp_relax_dims), outp_n_lmks);
  if (dbg.on) {
    dbg.dprintf("%s::%s UPDATE f_size=%d", dbg.fn, "update_factorization",
     outp_f_size);
    dbg.dprintf("%s::%s UPDATE is_mixed=%s", dbg.fn, "update_factorization",
     ew::Debug::to_str(outp_is_mixed));
    dbg.dprintf("%s::%s UPDATE is_reduced=%s", dbg.fn, "update_factorization",
     ew::Debug::to_str(outp_is_reduced));
  }
  outp_factorization.resize(outp_f_size * outp_f_size);
  outp_pivots.resize(outp_f_size);
  if (has_relax_params) {
    outp_relax_params.resize(3 * outp_n_lmks);
    int i = 0;
    while (i < outp_n_lmks) {
      const ew::Form3PointSet *psi = rhs_ps + outp_spline_to_ps[1][i];
      if (psi->relax_params.size() != 0) {
        for (int j = 0; j < psi->n; j += 1) {
          outp_relax_params[3 * (i + j) + 0] = psi->relax_params[3 * j + 0];
          outp_relax_params[3 * (i + j) + 1] = psi->relax_params[3 * j + 1];
          outp_relax_params[3 * (i + j) + 2] = psi->relax_params[3 * j + 2];
        }
      } else {
        for (int j = 0; j < psi->n; j += 1) {
          outp_relax_params[3 * (i + j) + 0] =
           outp_relax_params[3 * (i + j) + 1] =
           outp_relax_params[3 * (i + j) + 2] = 0;
        }
      }
      i += psi->n;
    }
  } else {
    outp_relax_params.resize(0);
  }
  if (outp_f_size > 0) {
    dbg.on && dbg.dprintf("%s::%s UPDATE ew::Tps3::factorize", dbg.fn,
     "update_factorization");
    ew::Tps3::factorize(VectorData(outp_factorization),
     VectorData(outp_pivots), &outp_nonsingularity,
     VectorData(outp_interaction), VectorData(outp_relax_dims),
     VectorData(outp_relax_params), outp_f_size, outp_is_mixed,
     outp_is_reduced, outp_n_lmks);
     outp_nonsingular = (outp_nonsingularity >= 1.0e-8);
  } else {
    outp_nonsingular = false;
    outp_nonsingularity = 0.0;
  }
  if (dbg.on) {
    dbg.dprintf("%s::%s UPDATE nonsingular=%s", dbg.fn, "update_factorization",
     ew::Debug::to_str(outp_nonsingular));
    dbg.dprintf("%s::%s }", dbg.fn, "update_factorization");
  }
}

void
ew::DataflowSpline3::reset_spline() const
{
  dbg.on && dbg.dprintf("%s::%s", dbg.fn, "reset_spline");
  VectorReset(outp_lmk_images);
  VectorReset(outp_spline);
  VectorReset(outp_relax_lmk_images);
}

void
ew::DataflowSpline3::update_spline() const
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "update_spline");
  if (!outp_nonsingular) {
    throw ew::ErrorRuntime("The thin-plate spline could not be caclulated"
     " (the equations are singular).");
  }
  const ew::DataflowForm3 *rhs = get_specimen();
  const ew::Form3PointSet *rhs_ps = &rhs->get_data()->pointsets[0];
  outp_spline.resize(3 * (outp_n_lmks + 4));
  outp_lmk_images.resize(3 * outp_n_lmks);
  outp_relax_lmk_images.resize(3 * outp_n_lmks);
  int i = 0;
  while (i < outp_n_lmks) {
    const ew::Form3PointSet *psi = rhs_ps + outp_spline_to_ps[1][i];
    if (psi->locations.size() == 0) {
      for (int j = 0; j < psi->n; j += 1) {
        outp_lmk_images[3 * (i + j) + 0] =
         outp_lmk_images[3 * (i + j) + 1] =
         outp_lmk_images[3 * (i + j) + 2] = 0.0;
      }
    } else {
      for (int j = 0; j < psi->n; j += 1) {
        outp_lmk_images[3 * (i + j) + 0] = psi->locations[3 * j + 0];
        outp_lmk_images[3 * (i + j) + 1] = psi->locations[3 * j + 1];
        outp_lmk_images[3 * (i + j) + 2] = psi->locations[3 * j + 2];
      }
    }
    i += psi->n;
  }
  dbg.on && dbg.dprintf("%s::%s   UPDATE ew::Tps3::solve", dbg.fn,
   "update_spline");
  ew::Tps3::solve(VectorData(outp_spline), &outp_energy,
   VectorData(outp_relax_lmk_images),
   VectorData(outp_factorization), VectorData(outp_pivots),
   VectorData(outp_lmk_images), VectorData(outp_relax_dims),
   VectorData(outp_relax_params), VectorData(outp_interaction),
   outp_n_lmks, outp_f_size, outp_is_mixed, outp_is_reduced);
  if (dbg.on) {
    dbg.dprintf("%s::%s UPDATE nonsingular=%s", dbg.fn, "update_factorization",
     ew::Debug::to_str(outp_nonsingular));
    dbg.dprintf("%s::%s UPDATE energy=%g", dbg.fn, "update_factorization",
     outp_energy);
    dbg.dprintf("%s::%s }", dbg.fn, "update_spline");
  }
}

/// This applies the spline to an array of points.
/// If the spline is singular, a std::runtime_error is thrown.
/// @param[out] outp
///   Where to store the warped point coordinates.
/// @param[in] inp
///   Where the original point coordinates are stored.
/// @param[in] l
///   The number of points.

void
ew::DataflowSpline3::warp_points(double *outp, const double *inp, int l) const
{
  dbg.on && dbg.dprintf("%s::%s(...) {", dbg.fn, "warp_points");
  make_spline();
  dbg.on && dbg.dprintf("%s::%s   ew::Tps3::map", dbg.fn, "warp_points");
  ew::Tps3::map(outp, inp, l, VectorData(outp_spline),
   VectorData(outp_lmks), outp_n_lmks);
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "warp_points");
}

// wdkg 2008-2010

#include <cmath>
#include "ew/Transform3.h"
#include "TestTransform3.h"

namespace {
  const char Name[] = "test_transform3";
  const char Desc[] = "Exercise ew::Transform3.\n";
  const char *DebugVal = 0;
  const int DIM = 3;
  const int DIM10 = DIM * 10;
  const double Pi = 3.14159265358979323844;
// Deliberately not const.
  double ZeroDouble = 0.0;
  ewtst::TestTransform3 Tst;
}

void
ewtst::TestTransform3::random_transform(ew::Transform3 *t)
{
  for (int i = 0; i < DIM; i += 1) {
    t->translate[i] = (random_double() - 0.5) * 2.0;
    for (int j = 0; j < DIM; j += 1) {
      t->orthog[i][j] = (random_double() - 0.5) * 2.0;
    }
  }
  t->scale = (random_double() + 0.01) * 2.0;
  t->set_to_normalization(t);
}

void
ewtst::TestTransform3::check_apply(const ew::Transform3 *tr, double *d,
 const double *de)
{
  float f[DIM10];
  for (int i = 0; i < DIM10; i += 1) {
    f[i] = d[i];
  }
  float fa[DIM10];
  double da[DIM10];
  for (int i = 0; i < DIM10; i += DIM) {
    tr->apply(da + i, d + i);
    tr->apply(fa + i, f + i);
  }
  for (int i = 0; i < DIM10; i += 1) {
    if (std::fabs(de[i] - da[i]) > 1e-5 || std::fabs(de[i] - fa[i]) > 1e-3) {
      report_error("apply");
      break;
    }
  }
  for (int i = 0; i < DIM10; i += DIM) {
    for (int j = 0; j < DIM; j += 1) {
      da[i + j] = d[i + j];
      fa[i + j] = f[i + j];
    }
    tr->apply(da + i, da + i);
    tr->apply(fa + i, fa + i);
  }
  for (int i = 0; i < DIM10; i += 1) {
    if (std::fabs(de[i] - da[i]) > 1e-5 || std::fabs(de[i] - fa[i]) > 1e-3) {
      report_error("apply-inplace");
      break;
    }
  }
}

bool
ewtst::TestTransform3::transforms_differ(const ew::Transform3 *tr,
 const ew::Transform3 *te) const
{
  for (int i = 0; i < DIM; i += 1) {
    if (std::fabs(tr->translate[i] - te->translate[i]) > 1e-5) {
      return true;
    }
    for (int j = 0; j < DIM; j += 1) {
      if (std::fabs(tr->orthog[i][j] - te->orthog[i][j]) > 1e-5) {
        return true;
      }
    }
  }
  if (std::fabs(tr->scale - te->scale) > 1e-5) {
    return true;
  }
  return false;
}

double
ewtst::TestTransform3::determinant(const ew::Transform3 *tr) const
{
  return
   tr->orthog[0][0] *
   (tr->orthog[1][1] * tr->orthog[2][2] - tr->orthog[2][1] * tr->orthog[1][2]) -
   tr->orthog[0][1] *
   (tr->orthog[1][0] * tr->orthog[2][2] - tr->orthog[2][0] * tr->orthog[1][2]) +
   tr->orthog[0][2] *
   (tr->orthog[1][0] * tr->orthog[2][1] - tr->orthog[2][0] * tr->orthog[1][1]);
}

const char *
ewtst::TestTransform3::test01(bool return_name)
{
  if (return_name) {
    return "translate";
  }
  ew::Transform3 tr;
  tr.set_to_identity();
  tr.translate[0] = 3.0;
  tr.translate[1] = -5.0;
  tr.translate[2] = 1.0;
  double d[DIM10];
  for (int i = 0; i < DIM10; i += 1) {
    d[i] = (random_double() - 0.5) * 20.0;
  }
  double de[DIM10];
  for (int i = 0; i < DIM10; i += DIM) {
    de[i] = d[i] + 3.0;
    de[i + 1] = d[i + 1] - 5.0;
    de[i + 2] = d[i + 2] + 1.0;
  }
  check_apply(&tr, d, de);
  return 0;
}

const char *
ewtst::TestTransform3::test02(bool return_name)
{
  if (return_name) {
    return "scale";
  }
  ew::Transform3 tr;
  tr.set_to_identity();
  tr.scale = 0.01;
  double d[DIM10];
  for (int i = 0; i < DIM10; i += 1) {
    d[i] = (random_double() - 0.5) * 20.0;
  }
  double de[DIM10];
  for (int i = 0; i < DIM10; i += DIM) {
    for (int j = 0; j < DIM; j += 1) {
      de[i + j] = d[i + j] * 0.01;
    }
  }
  check_apply(&tr, d, de);
  return 0;
}

const char *
ewtst::TestTransform3::test03(bool return_name)
{
  if (return_name) {
    return "rotate";
  }
  ew::Transform3 tr;
  tr.set_to_identity();
  tr.orthog[0][0] = 0.6;
  tr.orthog[0][1] = 0.48;
  tr.orthog[0][2] = 0.64;
  tr.orthog[1][0] = 0.0;
  tr.orthog[1][1] = 0.8;
  tr.orthog[1][2] = -0.6;
  tr.orthog[2][0] = -0.8;
  tr.orthog[2][1] = 0.36;
  tr.orthog[2][2] = 0.48;
  double d[DIM10];
  for (int i = 0; i < DIM10; i += 1) {
    d[i] = (random_double() - 0.5) * 20.0;
  }
  double de[DIM10];
  for (int i = 0; i < DIM10; i += DIM) {
    for (int j = 0; j < DIM; j += 1) {
      de[i + j] = 0.0;
      for (int k = 0; k < DIM; k += 1) {
        de[i + j] += d[i + k] * tr.orthog[j][k];
      }
    }
  }
  check_apply(&tr, d, de);
  return 0;
}

const char *
ewtst::TestTransform3::test04(bool return_name)
{
  if (return_name) {
    return "set_to_identity";
  }
  ew::Transform3 tr;
  tr.set_to_identity();
  double d[DIM10];
  for (int i = 0; i < DIM10; i += 1) {
    d[i] = (random_double() - 0.5) * 20.0;
  }
  double de[DIM10];
  for (int i = 0; i < DIM10; i += DIM) {
    for (int j = 0; j < DIM; j += 1) {
      de[i + j] = d[i + j];
    }
  }
  check_apply(&tr, d, de);
  return 0;
}

const char *
ewtst::TestTransform3::test05(bool return_name)
{
  if (return_name) {
    return "decomposition";
  }
  ew::Transform3 tr;
  random_transform(&tr);
  ew::Transform3 tt;
  tt.set_to_identity();
  ew::Transform3 ts;
  ts.set_to_identity();
  ew::Transform3 ta;
  ta.set_to_identity();
  for (int i = 0; i < DIM; i += 1) {
    tt.translate[i] = tr.translate[i];
    for (int j = 0; j < DIM; j += 1) {
      ta.orthog[i][j] = tr.orthog[i][j];
    }
  }
  ts.scale = tr.scale;
  ew::Transform3 te;
  te.set_to_composition(&ts, &ta);
  te.set_to_composition(&tt, &te);
  if (transforms_differ(&tr, &te)) {
    report_error("translate * scale * rotate");
  }
  te.set_to_composition(&ta, &ts);
  te.set_to_composition(&tt, &te);
  if (transforms_differ(&tr, &te)) {
    report_error("translate * rotate * scale");
  }
  return 0;
}

const char *
ewtst::TestTransform3::test06(bool return_name)
{
  if (return_name) {
    return "set_to_inverse";
  }
  ew::Transform3 te;
  te.set_to_identity();
  ew::Transform3 t;
  random_transform(&t);
  ew::Transform3 ti;
  ti.set_to_inverse(&t);
  ew::Transform3 tr;
  tr.set_to_composition(&t, &ti);
  if (transforms_differ(&tr, &te)) {
    report_error("t * ti");
  }
  tr.set_to_composition(&ti, &t);
  if (transforms_differ(&tr, &te)) {
    report_error("ti * t");
  }
  ti = t;
  ti.set_to_inverse(&ti);
  tr.set_to_composition(&t, &ti);
  if (transforms_differ(&tr, &te)) {
    report_error("t * ti (inplace)");
  }
  tr.set_to_composition(&ti, &t);
  if (transforms_differ(&tr, &te)) {
    report_error("ti * t (inplace)");
  }
  return 0;
}

const char *
ewtst::TestTransform3::test07(bool return_name)
{
  if (return_name) {
    return "set_to_composition";
  }
  ew::Transform3 a;
  random_transform(&a);
  ew::Transform3 b;
  random_transform(&b);
  ew::Transform3 tr;
  tr.set_to_composition(&a, &b);
  double d[DIM10];
  for (int i = 0; i < DIM10; i += 1) {
    d[i] = (random_double() - 0.5) * 20.0;
  }
  double de[DIM10];
  for (int i = 0; i < DIM10; i += DIM) {
    b.apply(de + i, d + i);
    a.apply(de + i, de + i);
  }
  check_apply(&tr, d, de);
  tr = a;
  tr.set_to_composition(&tr, &b);
  check_apply(&tr, d, de);
  tr = b;
  tr.set_to_composition(&a, &tr);
  check_apply(&tr, d, de);
  tr = a;
  tr.set_to_composition(&tr, &tr);
  for (int i = 0; i < DIM10; i += DIM) {
    a.apply(de + i, d + i);
    a.apply(de + i, de + i);
  }
  check_apply(&tr, d, de);
  return 0;
}

const char *
ewtst::TestTransform3::test08(bool return_name)
{
  if (return_name) {
    return "get_matrix_gl";
  }
  ew::Transform3 tr;
  random_transform(&tr);
  double aa[16];
  tr.get_matrix_gl(aa);
  double d[DIM10];
  for (int i = 0; i < DIM10; i += 1) {
    d[i] = (random_double() - 0.5) * 20.0;
  }
  for (int i = 0; i < DIM10; i += DIM) {
    double da[DIM];
    tr.apply(da, d + i);
    double b[4];
    b[0] = aa[0] * d[i] + aa[4] * d[i + 1] + aa[8] * d[i + 2] + aa[12] * 1.0;
    b[1] = aa[1] * d[i] + aa[5] * d[i + 1] + aa[9] * d[i + 2] + aa[13] * 1.0;
    b[2] = aa[2] * d[i] + aa[6] * d[i + 1] + aa[10] * d[i + 2] + aa[14] * 1.0;
    b[3] = aa[3] * d[i] + aa[7] * d[i + 1] + aa[11] * d[i + 2] + aa[15] * 1.0;
    if (std::fabs(b[0] - da[0]) > 1e-5 ||
     std::fabs(b[1] - da[1]) > 1e-5 ||
     std::fabs(b[2] - da[2]) > 1e-5 ||
     std::fabs(b[3] - 1.0) > 1e-5) {
      report_error("application");
    }
  }
  return 0;
}

const char *
ewtst::TestTransform3::test09(bool return_name)
{
  if (return_name) {
    return "get_valid";
  }
  double f = 1.0;
  while (f / 10.0 != 0) {
    f = f / 10.0;
  }
  ew::Transform3 tr;
  tr.set_to_identity();
  if (!tr.get_valid()) {
    report_error("identity");
  }
  for (int i = 0; i < DIM; i += 1) {
    tr.translate[i] = (random_double() - 0.5) * 1e5;
    for (int j = 0; j < DIM; j += 1) {
      tr.orthog[i][j] = (random_double() - 0.5) * 1e5;
    }
  }
  tr.scale = (random_double() + 0.5) * 1e5;
  if (!tr.get_valid()) {
    report_error("random");
  }
  tr.orthog[0][1] = f;
  if (!tr.get_valid()) {
    report_error("subnormal");
  }
  tr.orthog[0][1] = 1.0 / ZeroDouble;
  if (tr.get_valid()) {
    report_error("inf");
  }
  tr.orthog[0][1] = 1.0;
  tr.translate[1] = 0.0 / ZeroDouble;
  if (tr.get_valid()) {
    report_error("nan");
  }
  tr.translate[1] = 1.0;
  tr.scale = 0.0;
  if (tr.get_valid()) {
    report_error("scale zero");
  }
  tr.scale = 1.0 / ZeroDouble;
  if (tr.get_valid()) {
    report_error("scale inf");
  }
  tr.scale = 0.0 / ZeroDouble;
  if (tr.get_valid()) {
    report_error("scale nan");
  }
  tr.scale = -1.0;
  if (tr.get_valid()) {
    report_error("scale negative");
  }
  tr.scale = f;
  if (tr.get_valid()) {
    report_error("scale subnormal");
  }
  return 0;
}

const char *
ewtst::TestTransform3::test10(bool return_name)
{
  if (return_name) {
    return "get_denormalization";
  }
  ew::Transform3 tr;
  tr.set_to_identity();
  if (std::fabs(tr.get_denormalization()) > 1e-5) {
    report_error("identity");
  }
  tr.orthog[0][0] = 0.1;
  tr.orthog[0][1] = 0.2;
  tr.orthog[0][2] = 0.3;
  tr.orthog[1][0] = 0.4;
  tr.orthog[1][1] = 0.5;
  tr.orthog[1][2] = 0.6;
  tr.orthog[2][0] = 0.7;
  tr.orthog[2][1] = 0.8;
  tr.orthog[2][2] = 0.9;
  tr.set_to_normalization(&tr);
  if (std::fabs(tr.get_denormalization()) > 1e-5) {
    report_error("normalized");
  }
  for (int i = 0; i < 100; i += 1) {
    random_transform(&tr);
    if (std::fabs(tr.get_denormalization()) > 1e-5) {
      report_error("normalized random");
      break;
    }
  }
  tr.set_to_identity();
  tr.orthog[0][0] = 0.6;
  tr.orthog[0][1] = 0.48;
  tr.orthog[0][2] = 0.64;
  tr.orthog[1][0] = 0.0;
  tr.orthog[1][1] = 0.8;
  tr.orthog[1][2] = -0.6;
  tr.orthog[2][0] = -0.8;
  tr.orthog[2][1] = 0.36;
  tr.orthog[2][2] = 0.48;
  if (std::fabs(tr.get_denormalization()) > 1e-5) {
    report_error("orthog");
  }
  tr.set_to_identity();
  tr.orthog[0][0] = 0.7;
  tr.orthog[0][1] = 0.48;
  tr.orthog[0][2] = 0.64;
  tr.orthog[1][0] = 0.0;
  tr.orthog[1][1] = 0.8;
  tr.orthog[1][2] = -0.6;
  tr.orthog[2][0] = -0.8;
  tr.orthog[2][1] = 0.36;
  tr.orthog[2][2] = 0.48;
  if (std::fabs(tr.get_denormalization()) < 0.05) {
    report_error("not orthog");
  }
  tr.orthog[0][0] = 0.6;
  tr.orthog[0][1] = 0.48;
  tr.orthog[0][2] = 0.64;
  tr.orthog[1][0] = 0.0;
  tr.orthog[1][1] = 0.6;
  tr.orthog[1][2] = -0.6;
  tr.orthog[2][0] = -0.8;
  tr.orthog[2][1] = 0.36;
  tr.orthog[2][2] = 0.48;
  if (std::fabs(tr.get_denormalization()) < 0.05) {
    report_error("not orthog again");
  }
  tr.orthog[0][0] = 0.6;
  tr.orthog[0][1] = 0.48;
  tr.orthog[0][2] = 0.64;
  tr.orthog[1][0] = 0.48;
  tr.orthog[1][1] = 0.64;
  tr.orthog[1][2] = 0.6;
  tr.orthog[2][0] = 0.64;
  tr.orthog[2][1] = 0.6;
  tr.orthog[2][2] = 0.48;
  if (std::fabs(tr.get_denormalization()) < 0.05) {
    report_error("symmetric");
  }
  ew::Transform3 t = tr;
  tr.set_to_normalization(&t);
  if (std::fabs(tr.get_denormalization()) > 1e-5) {
    report_error("normalized");
  }
  return 0;
}

const char *
ewtst::TestTransform3::test11(bool return_name)
{
  if (return_name) {
    return "automatic normalization";
  }
  ew::Transform3 t0;
  while (1) {
    random_transform(&t0);
    if (determinant(&t0) > 0) {
      break;
    }
  }
  t0.translate[0] = 0.0;
  t0.translate[1] = 0.0;
  t0.translate[2] = 0.0;
  t0.scale = 1.0;
  ew::Transform3 tr;
  tr = t0;
  for (int i = 0; i < 100; i += 1) {
    tr.set_to_composition(&tr, &tr);
    tr.comps_cnt = 0;
  }
  bool all_zero = true;
  for (int j = 0; j < DIM; j += 1) {
    for (int k = 0; k < DIM; k += 1) {
      if (tr.orthog[j][k] != 0.0) {
        all_zero = false;
        break;
      }
    }
    if (!all_zero) {
      break;
    }
  }
  if (tr.get_valid() && std::isfinite(tr.get_denormalization()) && !all_zero) {
    report_error("auto normalization unnecessary");
  }
  tr = t0;
  for (int i = 0; i < 100; i += 1) {
    tr.set_to_composition(&tr, &tr);
  }
  if (!tr.get_valid() || !std::isfinite(tr.get_denormalization()) ||
   tr.get_denormalization() > 1e-5) {
    report_error("auto normalization failed");
  }
  return 0;
}

// Check set_to_interpolation() is:
//   correct for 0.0 and 1.0
//   continuous, affine in translation, monotone in scale and rotation
//   exponential in scale and rotation

const char *
ewtst::TestTransform3::test12(bool return_name)
{
  if (return_name) {
    return "set_to_interpolation";
  }
  ew::Transform3 a;
  ew::Transform3 b;
  ew::Transform3 tr;
  ew::Transform3 te;
  for (int i = 0; i < 2; i += 1) {
    if (i == 0) {
// Check between 2 reflections.
      while (1) {
        random_transform(&a);
        if (determinant(&a) < 0) {
          break;
        }
      }
      while (1) {
        random_transform(&b);
        if (determinant(&b) < 0) {
          break;
        }
      }
    } else if (i == 1) {
// Check between rotations with a large angular difference.
      while (1) {
        random_transform(&a);
        if (determinant(&a) > 0) {
          break;
        }
      }
      random_transform(&b);
      ew::Transform3 ro;
      ro.set_to_identity();
      ro.orthog[0][0] = std::cos(Pi * 0.95);
      ro.orthog[0][1] = std::sin(Pi * 0.95);
      ro.orthog[1][0] = -ro.orthog[0][1];
      ro.orthog[1][1] = ro.orthog[0][0];
      tr.set_to_composition(&a, &ro);
      for (int j = 0; j < DIM; j += 1) {
        for (int k = 0; k < DIM; k += 1) {
          b.orthog[j][k] = tr.orthog[j][k];
        }
      }
    }
    te = a;
    tr = b;
    if (!tr.set_to_interpolation(&a, &tr, 0.0)) {
      report_error("%d e = 0 not smooth", i);
    }
    if (transforms_differ(&tr, &te)) {
      report_error("e = 0");
    }
    ew::Transform3 tr0 = tr;
    te = b;
    tr = a;
    if (!tr.set_to_interpolation(&tr, &b, 1.0)) {
      report_error("%d e = 1 not smooth", i);
    }
    if (transforms_differ(&tr, &te)) {
      report_error("e = 1");
    }
    ew::Transform3 tr1 = tr;
    int first = 1;
    ew::Transform3 ltr;
    for (double e = -0.5; e < 1.5; e += 0.001) {
      if (!tr.set_to_interpolation(&a, &b, e)) {
        report_error("%d cont not smooth", i);
      }
      if (first) {
        first = 0;
        ltr = tr;
        continue;
      }
      bool cont_error = false;
      for (int j = 0; j < DIM; j += 1) {
        for (int k = 0; k < DIM; k += 1) {
          if (std::fabs(tr.orthog[j][k] - ltr.orthog[j][k]) > 1e-2) {
            cont_error = true;
          }
        }
        if (std::fabs(tr.translate[j] - ltr.translate[j]) > 1e-2) {
          cont_error = true;
        }
      }
      if ((e >= 0.0 && e < 1.0 && std::fabs(tr.scale - ltr.scale) > 1e-2) ||
       (!(e >= 0.0 && e < 1.0) && std::fabs(tr.scale - ltr.scale) > 1e-1)) {
        cont_error = true;
      }
      if (cont_error) {
        report_error("continuity");
      }
      if (e > 0.0 && e < 1.0) {
        double d1 = tr.translate[0] - ltr.translate[0];
        double d2 = tr1.translate[0] - tr0.translate[0];
        if (std::fabs(d1 - d2 / 1000.0) > 1e-8) {
          report_error("linear translate x");
        }
        d1 = tr.translate[1] - ltr.translate[1];
        d2 = tr1.translate[1] - tr0.translate[1];
        if (std::fabs(d1 - d2 / 1000.0) > 1e-8) {
          report_error("linear translate y");
        }
        d1 = tr.translate[2] - ltr.translate[2];
        d2 = tr1.translate[2] - tr0.translate[2];
        if (std::fabs(d1 - d2 / 1000.0) > 1e-8) {
          report_error("linear translate z");
        }
        if ((tr1.scale > tr0.scale && tr.scale < ltr.scale) ||
         (tr1.scale < tr0.scale && tr.scale > ltr.scale)) {
          report_error("monotone scale");
        }
        if ((tr.orthog[0][0] - ltr.orthog[0][0]) *
         (tr1.orthog[0][0] - tr0.orthog[0][0]) +
         (tr.orthog[1][0] - ltr.orthog[1][0]) *
         (tr1.orthog[1][0] - tr0.orthog[1][0]) +
         (tr.orthog[2][0] - ltr.orthog[2][0]) *
         (tr1.orthog[2][0] - tr0.orthog[2][0]) < -1e-8) {
          report_error("axis x");
        }
        if ((tr.orthog[0][1] - ltr.orthog[0][1]) *
         (tr1.orthog[0][1] - tr0.orthog[0][1]) +
         (tr.orthog[1][1] - ltr.orthog[1][1]) *
         (tr1.orthog[1][1] - tr0.orthog[1][1]) +
         (tr.orthog[2][1] - ltr.orthog[2][1]) *
         (tr1.orthog[2][1] - tr0.orthog[2][1]) < -1e-8) {
          report_error("axis y");
        }
        if ((tr.orthog[0][2] - ltr.orthog[0][2]) *
         (tr1.orthog[0][2] - tr0.orthog[0][2]) +
         (tr.orthog[1][2] - ltr.orthog[1][2]) *
         (tr1.orthog[1][2] - tr0.orthog[1][2]) +
         (tr.orthog[2][2] - ltr.orthog[2][2]) *
         (tr1.orthog[2][2] - tr0.orthog[2][2]) < -1e-8) {
          report_error("axis z");
        }
      }
      ltr = tr;
    }
    if (!tr.set_to_interpolation(&a, &b, 1.0 / 3.0)) {
      report_error("%d 1/3 not smooth", i);
    }
    ew::Transform3 ti;
    ti.set_to_inverse(&a);
    ew::Transform3 tc;
    tc.set_to_composition(&tr, &ti);
    ew::Transform3 tc3;
    tc3.set_to_composition(&tc, &tc);
    tc3.set_to_composition(&tc, &tc3);
    tr.set_to_composition(&tc3, &a);
    for (int j = 0; j < DIM; j += 1) {
      tr.translate[j] = b.translate[j];
    }
    te = b;
    if (transforms_differ(&tr, &te)) {
      report_error("e = 1 / 3");
    }
  }
// Check start and end the same.
  random_transform(&a);
  te = a;
  if (!tr.set_to_interpolation(&a, &a, 0.5)) {
    report_error("b = a, e = 0.5 not smooth");
  }
  if (transforms_differ(&tr, &te)) {
    report_error("b = a, e = 0.5");
  }
  tr = a;
  if (!tr.set_to_interpolation(&tr, &tr, 0.5)) {
    report_error("b = a, e = 0.5 (inplace) not smooth");
  }
  if (transforms_differ(&tr, &te)) {
    report_error("a == b, e = 0.5 (inplace)");
  }
  if (!tr.set_to_interpolation(&a, &a, 5.5)) {
    report_error("b = a, e = 5.5 not smooth");
  }
  if (transforms_differ(&tr, &te)) {
    report_error("b = a, e = 5.5");
  }
// Check one refelection, one rotation leads to an exception.
  while (1) {
    random_transform(&a);
    if (determinant(&a) > 0) {
      break;
    }
  }
  while (1) {
    random_transform(&b);
    if (determinant(&b) < 0) {
      break;
    }
  }
  if (tr.set_to_interpolation(&a, &b, 0.5)) {
    report_error("refl smooth");
  }
// Check a rotation of 180 degress results in a false return value.
  ew::Transform3 ro1;
  ro1.set_to_identity();
  ro1.orthog[0][0] = -1.0;
  ro1.orthog[0][1] = 0.0;
  ro1.orthog[1][0] = -ro1.orthog[0][1];
  ro1.orthog[1][1] = ro1.orthog[0][0];
  tr.set_to_composition(&a, &ro1);
  for (int i = 0; i < DIM; i += 1) {
    for (int j = 0; j < DIM; j += 1) {
      b.orthog[i][j] = tr.orthog[i][j];
    }
  }
  if (tr.set_to_interpolation(&a, &b, 0.5)) {
    report_error("180 smooth");
  }
// Check a rotation close to 180 degress results in a false return value.
  ew::Transform3 ro2;
  ro2.set_to_identity();
  ro2.orthog[0][0] = std::cos(Pi * 0.9999);
  ro2.orthog[0][1] = std::sin(Pi * 0.9999);
  ro2.orthog[1][0] = -ro2.orthog[0][1];
  ro2.orthog[1][1] = ro2.orthog[0][0];
  tr.set_to_composition(&a, &ro2);
  for (int i = 0; i < DIM; i += 1) {
    for (int j = 0; j < DIM; j += 1) {
      b.orthog[i][j] = tr.orthog[i][j];
    }
  }
  if (tr.set_to_interpolation(&a, &b, 0.5)) {
    report_error("~180 smooth");
  }
  return 0;
}

const char *
ewtst::TestTransform3::test(int n, bool return_name)
{
  switch(n) {
  case 1:
    return test01(return_name);
  case 2:
    return test02(return_name);
  case 3:
    return test03(return_name);
  case 4:
    return test04(return_name);
  case 5:
    return test05(return_name);
  case 6:
    return test06(return_name);
  case 7:
    return test07(return_name);
  case 8:
    return test08(return_name);
  case 9:
    return test09(return_name);
  case 10:
    return test10(return_name);
  case 11:
    return test11(return_name);
  case 12:
    return test12(return_name);
  }
  return 0;
}

ewtst::TestTransform3::TestTransform3() :
 Test(Name, Desc, DebugVal)
{
}

int
ewtst::TestTransform3::exec(int ac, char **av)
{
  return Tst.exec_base(ac, av);
}

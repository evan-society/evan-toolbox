// wdkg 2008-2010

#include <cmath>
#include "ew/Bbox3.h"
#include "ew/Debug.h"
#include "ew/Time.h"
#include "src/Geom3.h"
#include "TestGeom3.h"

namespace {
  const char Name[] = "test_geom3";
  const char Desc[] = "Exercise ew::Geom.\n";
  const char DebugVal[] = "timing";
  ewtst::TestGeom3 Tst;
}

void
ewtst::TestGeom3::chk_nearest_tri_pt(double *tri, double *pt, bool known_coeffs,
 double *kc)
{
  double coeffs[3];
  double proj[3];
  double d = ew::Geom3::nearest_on_polytope(proj, coeffs, pt, tri, 3);
  if (coeffs[0] < 0.0 || coeffs[0] > 1.0 ||
   coeffs[1] < 0.0 || coeffs[1] > 1.0 ||
   coeffs[2] < 0.0 || coeffs[2] > 1.0 ||
   std::fabs(coeffs[0] + coeffs[1] + coeffs[2] - 1.0) > 1e-10) {
    report_error("bad coeffs");
  }
  double proj1[3];
  ew::Geom3::linear_combination(proj1, coeffs[0], tri, coeffs[1], tri + 3,
   coeffs[2], tri + 6);
  double s[3];
  ew::Geom3::subtract(s, proj, proj1);
  if (ew::Geom3::norm(s) > 1e-15) {
    report_error("bad proj");
  }
  ew::Geom3::subtract(s, proj, pt);
  if (std::fabs(d - ew::Geom3::norm(s)) > 1e-13) {
    report_error("bad d %g %g %g", d, ew::Geom3::norm(s),
     std::fabs(d - ew::Geom3::norm(s)));
  }
  double coeffs1[3];
  for (int i = 0; i < 3; i += 1) {
    for (int j = -1; j <= 1; j += 2) {
      coeffs1[0] = coeffs[0];
      coeffs1[1] = coeffs[1];
      coeffs1[2] = coeffs[2];
      coeffs1[i] += j * 1e-3;
      coeffs1[(i + 1) % 3] -= j * 1e-3;
      if (coeffs1[0] >= 0.0 && coeffs1[0] <= 1.0 &&
       coeffs1[1] >= 0.0 && coeffs1[1] <= 1.0 &&
       coeffs1[2] >= 0.0 && coeffs1[2] <= 1.0) {
        ew::Geom3::linear_combination(proj1, coeffs1[0], tri, coeffs1[1],
         tri + 3, coeffs1[2], tri + 6);
        ew::Geom3::subtract(s, proj1, pt);
        if (ew::Geom3::norm(s) < d - 1.0e-13) {
          report_error("not minimal %g %g %g", d, ew::Geom3::norm(s),
           ew::Geom3::norm(s) - d);
        }
      }
    }
  }
  if (known_coeffs) {
    if (std::fabs(coeffs[0] - kc[0]) > 1e-13 ||
     std::fabs(coeffs[1] - kc[1]) > 1e-13 ||
     std::fabs(coeffs[2] - kc[2]) > 1e-13) {
      report_error("unexpected coeffs exp %g %g %g was %g %g %g", kc[0],
       kc[1], kc[2], coeffs[0], coeffs[1], coeffs[2]);
    }
  }
}

void
ewtst::TestGeom3::chk_nearest_tri(double *tri, bool regular)
{
  double pt[3];
  double kc[3];
  for (int i = 0; i < 3; i += 1) {
    for (int j = 0; j < 3; j += 1) {
      pt[j] = random_double();
    }
    chk_nearest_tri_pt(tri, pt, false, kc);
  }
  for (int i = 0; i < 3; i += 1) {
    for (int j = 0; j < 3; j += 1) {
      pt[j] = random_double() * 100.0;
    }
    chk_nearest_tri_pt(tri, pt, false, kc);
  }
  if (!regular) {
    return;
  }
  double d1[3];
  ew::Geom3::subtract(d1, tri + 3, tri);
  double d2[3];
  ew::Geom3::subtract(d2, tri + 6, tri);
  double cr[3];
  ew::Geom3::cross(cr, d1, d2);
  for (int i = 0; i < 9; i += 1) {
    for (int j = 0; j < 19; j += 1) {
      switch (j) {
      case 0:
        kc[0] = 1.0;
        kc[1] = 0.0;
        kc[2] = 0.0;
        break;
      case 1:
        kc[0] = 0.0;
        kc[1] = 1.0;
        kc[2] = 0.0;
        break;
      case 2:
        kc[0] = 0.0;
        kc[1] = 0.0;
        kc[2] = 1.0;
        break;
      case 3:
        kc[0] = 0.3;
        kc[1] = 0.7;
        kc[2] = 0.0;
        break;
      case 4:
        kc[0] = 0.0;
        kc[1] = 0.3;
        kc[2] = 0.7;
        break;
      case 5:
        kc[0] = 0.7;
        kc[1] = 0.0;
        kc[2] = 0.3;
        break;
      case 6:
        kc[0] = 0.5;
        kc[1] = 0.2;
        kc[2] = 0.3;
        break;
      case 7:
        kc[0] = 0.0;
        kc[1] = -0.1;
        kc[2] = 1.1;
        break;
      case 8:
        kc[0] = 1.1;
        kc[1] = 0.0;
        kc[2] = -0.1;
        break;
      case 9:
        kc[0] = -0.1;
        kc[1] = 1.1;
        kc[2] = 0.0;
        break;
      case 10:
        kc[0] = 0.0;
        kc[1] = 0.4;
        kc[2] = -1.4;
        break;
      case 11:
        kc[0] = -1.4;
        kc[1] = 0.0;
        kc[2] = 0.4;
        break;
      case 12:
        kc[0] = 0.4;
        kc[1] = -1.4;
        kc[2] = 0.0;
        break;
      case 13:
        kc[0] = 0.5;
        kc[1] = -0.2;
        kc[2] = 0.7;
        break;
      case 14:
        kc[0] = 0.7;
        kc[1] = 0.5;
        kc[2] = -0.2;
        break;
      case 15:
        kc[0] = -0.2;
        kc[1] = 0.7;
        kc[2] = 0.5;
        break;
      case 16:
        kc[0] = -0.2;
        kc[1] = -0.1;
        kc[2] = 1.3;
        break;
      case 17:
        kc[0] = 1.3;
        kc[1] = -0.2;
        kc[2] = -0.1;
        break;
      case 18:
        kc[0] = -0.1;
        kc[1] = 1.3;
        kc[2] = -0.2;
        break;
      }
      for (int k = 0; k < 7; k += 1) {
        double kc1[3];
        switch (k) {
        case 0:
          kc1[0] = kc[0];
          kc1[1] = kc[1];
          kc1[2] = kc[2];
          break;
        case 1:
          kc1[0] = kc[0] + 0.01;
          kc1[1] = kc[1] - 0.01;
          kc1[2] = kc[2];
          break;
        case 2:
          kc1[0] = kc[0] - 0.01;
          kc1[1] = kc[1] + 0.01;
          kc1[2] = kc[2];
          break;
        case 3:
          kc1[0] = kc[0];
          kc1[1] = kc[1] + 0.01;
          kc1[2] = kc[2] - 0.01;
          break;
        case 4:
          kc1[0] = kc[0];
          kc1[1] = kc[1] - 0.01;
          kc1[2] = kc[2] + 0.01;
          break;
        case 5:
          kc1[0] = kc[0] + 0.01;
          kc1[1] = kc[1];
          kc1[2] = kc[2] - 0.01;
          break;
        case 6:
          kc1[0] = kc[0] - 0.01;
          kc1[1] = kc[1];
          kc1[2] = kc[2] + 0.01;
          break;
        }
        double p1[3];
        ew::Geom3::linear_combination(p1, kc1[0], tri, kc1[1], tri + 3,
         kc1[2], tri + 6);
        double p2[3];
        static const double Fact[9] = {-100.0, -1.0, -0.01, -1.0e-10, 0.0,
         1.0e-10, 0.01, 1.0, 1.00};
        ew::Geom3::linear_combination(p2, 1.0, p1, Fact[i], cr);
        chk_nearest_tri_pt(tri, p2,
         (kc1[0] >= 0.0 && kc1[1] >= 0.0 && kc1[2] >= 0.0), kc1);
      }
    }
  }
}

const char *
ewtst::TestGeom3::test01(bool return_name)
{
  if (return_name) {
    return "nearest_on_polytope: regular";
  }
  double tri[9];
  for (int i = 0; i < 10; i += 1) {
    for (int j = 0; j < 9; j += 1) {
      tri[j] = random_double();
    }
    double d1[3];
    ew::Geom3::subtract(d1, tri + 3, tri);
    double d2[3];
    ew::Geom3::subtract(d2, tri + 6, tri);
    double cr[3];
    ew::Geom3::cross(cr, d1, d2);
    double d = ew::Geom3::dot(d1, d2);
    if (std::fabs(d) < 0.001) {
      i -= 1;
      continue;
    }
    chk_nearest_tri(tri, true);
  }
  return 0;
}

const char *
ewtst::TestGeom3::test02(bool return_name)
{
  if (return_name) {
    return "nearest_on_polytope: degenerate";
  }
  double tri[9];
  for (int i = 0; i < 10; i += 1) {
    for (int j = 0; j < 6; j += 1) {
      tri[j] = random_double();
    }
    for (int j = 6; j < 9; j += 1) {
      tri[j] = tri[j - 6];
    }
    chk_nearest_tri(tri, false);
  }
  return 0;
}

const char *
ewtst::TestGeom3::test03(bool return_name)
{
  if (return_name) {
    return "nearest_on_polytope: nearly degenerate";
  }
  double tri[9];
  for (int i = 0; i < 10; i += 1) {
    for (int j = 0; j < 6; j += 1) {
      tri[j] = random_double();
    }
    for (int j = 6; j < 9; j += 1) {
      tri[j] = tri[j - 6] + random_double() * 1e-10;
    }
    chk_nearest_tri(tri, false);
  }
  return 0;
}

const char *
ewtst::TestGeom3::test04(bool return_name)
{
  if (return_name) {
    return "nearest_on_polytope: fully degenerate";
  }
  double tri[9];
  for (int i = 0; i < 10; i += 1) {
    for (int j = 0; j < 3; j += 1) {
      tri[j] = random_double();
    }
    for (int j = 3; j < 9; j += 1) {
      tri[j] = tri[j - 3];
    }
    chk_nearest_tri(tri, false);
  }
  return 0;
}

const char *
ewtst::TestGeom3::test05(bool return_name)
{
  if (return_name) {
    return "nearest_on_polytope: nearly fully degenerate";
  }
  double tri[9];
  for (int i = 0; i < 10; i += 1) {
    for (int j = 0; j < 3; j += 1) {
      tri[j] = random_double();
    }
    for (int j = 3; j < 9; j += 1) {
      tri[j] = tri[j - 3] + random_double() * 1e-10;
    }
    chk_nearest_tri(tri, false);
  }
  return 0;
}

const char *
ewtst::TestGeom3::test06(bool return_name)
{
  if (return_name) {
    return "nearest_on_polytope: timing";
  }
  double tri[900];
  for (int i = 0; i < 900; i += 1) {
    tri[i] = random_double();
  }
  double pt[300];
  for (int j = 0; j < 300; j += 1) {
    pt[j] = random_double();
  }
  const ew::Debug &dbg = ew::Debug::instance();
  double t0 = ew::Time::elapsed();
  double proj[3];
  double coeffs[3];
  for (int i = 0; i < 900; i += 9) {
    for (int j = 0; j < 300; j += 3) {
      ew::Geom3::nearest_on_polytope(proj, coeffs, pt + j, tri + i, 3);
    }
  }
  double t1 = ew::Time::elapsed();
  if (dbg.get_active("timing")) {
    dbg.dprintf("%s nearest_on_polytope %d in %.6f average %.9f", "timing",
     10000, t1 - t0, (t1 - t0) / 10000);
  }
  return 0;
}

const char *
ewtst::TestGeom3::test07(bool return_name)
{
  if (return_name) {
    return "crop_polytope";
  }
  for (int rep = 0; rep < 10; rep += 1) {
    double tri[9];
    for (int j = 0; j < 9; j += 1) {
      tri[j] = random_double() + 1.0;
    }
    for (int div = 1; div <= 64; div *= 2) {
      for (int x = 0; x < div; x += 1) {
        for (int y = 0; y < div; y += 1) {
          for (int z = 0; z < div; z += 1) {
            double outp[85 * 3];
            ew::Bbox3 box;
            box.min[0] = x * 3.0 / div;
            box.min[1] = y * 3.0 / div;
            box.min[2] = z * 3.0 / div;
            box.max[0] = (x + 1) * 3.0 / div;
            box.max[1] = (y + 1) * 3.0 / div;
            box.max[2] = (z + 1) * 3.0 / div;
            int n = ew::Geom3::crop_polytope(outp, tri, 3, &box);
            if (n == 1 || n == 2 || n < 0 || n > 85) {
              report_error("bad output pts");
            }
            if (n == 0) {
              continue;
            }
            for (int i = 0; i < n; i += 1) {
              if (outp[3 * i + 0] < box.min[0] ||
               outp[3 * i + 0] > box.max[0] ||
               outp[3 * i + 1] < box.min[1] ||
               outp[3 * i + 1] > box.max[1] ||
               outp[3 * i + 2] < box.min[2] ||
               outp[3 * i + 2] > box.max[2]) {
                report_error("out of box");
              }
              if (outp[3 * i + 0] > box.min[0] &&
               outp[3 * i + 0] < box.max[0] &&
               outp[3 * i + 1] > box.min[1] &&
               outp[3 * i + 1] < box.max[1] &&
               outp[3 * i + 2] > box.min[2] &&
               outp[3 * i + 2] < box.max[2] &&
               (outp[3 * i + 0] != tri[0] ||
               outp[3 * i + 1] != tri[1] ||
               outp[3 * i + 2] != tri[2]) &&
               (outp[3 * i + 0] != tri[3] ||
               outp[3 * i + 1] != tri[4] ||
               outp[3 * i + 2] != tri[5]) &&
               (outp[3 * i + 0] != tri[6] ||
               outp[3 * i + 1] != tri[7] ||
               outp[3 * i + 2] != tri[8])) {
                report_error("inside box, non-vertex");
              }
              double proj[3];
              double coeffs[3];
              double d = ew::Geom3::nearest_on_polytope(proj, coeffs,
               outp + 3 * i, tri, 3);
              if (std::fabs(d) > 1e-10) {
                report_error("out of face");
              }
            }
            for (int i = 0; i < 3; i += 1) {
              if (tri[3 * i + 0] >= box.min[0] &&
               tri[3 * i + 0] <= box.max[0] &&
               tri[3 * i + 1] >= box.min[1] &&
               tri[3 * i + 1] <= box.max[1] &&
               tri[3 * i + 2] >= box.min[2] &&
               tri[3 * i + 2] <= box.max[2]) {
                bool found = false;
                for (int j = 0; j < n; j += 1) {
                  double s[3];
                  ew::Geom3::subtract(s, tri + 3 * i, outp + 3 * j);
                  if (ew::Geom3::norm(s) < 1e-10) {
                    found = true;
                    break;
                  }
                }
                if (!found) {
                  report_error("missing internal vertex");
                }
              }
            }
            for (int i = 0; i < 3; i += 1) {
              for (int j = 0; j < 3; j += 1) {
                for (int dir = 0; dir < 3; dir += 1) {
                  for (int k = 0; k < 2; k += 1) {
                    double cut;
                    if (k == 0) {
                      cut = box.min[dir];
                    } else {
                      cut = box.max[dir];
                    }
                    if (tri[i * 3 + dir] < cut &&
                     tri[j * 3 + dir] > cut) {
                      double ci = (tri[j * 3 + dir] - cut) /
                       (tri[j * 3 + dir] - tri[i * 3 + dir]);
                      double cj = (cut - tri[i * 3 + dir]) /
                       (tri[j * 3 + dir] - tri[i * 3 + dir]);
                      double inter[3];
                      ew::Geom3::linear_combination(inter, ci, tri + i * 3,
                       cj, tri + j * 3);
                      if ((dir == 0 || inter[0] > box.min[0]) &&
                       (dir == 0 || inter[0] < box.max[0]) &&
                       (dir == 1 || inter[1] > box.min[1]) &&
                       (dir == 1 || inter[1] < box.max[1]) &&
                       (dir == 2 || inter[2] > box.min[2]) &&
                       (dir == 2 || inter[2] < box.max[2])) {
                        bool found = false;
                        for (int l = 0; l < n; l += 1) {
                          double s[3];
                          ew::Geom3::subtract(s, inter, outp + 3 * l);
                          if (ew::Geom3::norm(s) < 1e-10) {
                            found = true;
                            break;
                          }
                        }
                        if (!found) {
                          report_error("missing edge - box face intersection");
                        }
                      }
                    }
                  }
                }
              }
            }
            for (int dir = 0; dir < 3; dir += 1) {
              for (int i = 0; i < 3; i += 1) {
                for (int j = 0; j < 3; j += 1) {
                  if (i == j || dir == i || dir == j) {
                    continue;
                  }
                  double a[3];
                  double b[3];
                  if (i == 0) {
                    a[i] = b[i] = box.min[i];
                  } else {
                    a[i] = b[i] = box.max[i];
                  }
                  if (j == 0) {
                    a[j] = b[j] = box.min[j];
                  } else {
                    a[j] = b[j] = box.max[j];
                  }
                  a[dir] = box.min[dir];
                  b[dir] = box.max[dir];
                  double v01[3];
                  ew::Geom3::subtract(v01, tri + 3, tri);
                  double v02[3];
                  ew::Geom3::subtract(v02, tri + 6, tri);
                  double ab[3];
                  ew::Geom3::subtract(ab, b, a);
                  double v0a[3];
                  ew::Geom3::subtract(v0a, a, tri);
                  double cr[3];
                  ew::Geom3::cross(cr, v01, v02);
                  double mt = ew::Geom3::dot(ab, cr);
                  double m0 = ew::Geom3::dot(v0a, cr);
                  if (std::fabs(m0) >= std::fabs(mt)) {
                    continue;
                  }
                  double cb = -m0 / mt;
                  double ca = 1.0 - cb;
                  if (ca < 1e-10 || ca > 1.0 - 1e-10 ||
                   cb < 1e-10 || cb > 1.0 - 1e-10) {
                    continue;
                  }
                  double q[3];
                  ew::Geom3::linear_combination(q, ca, a, cb, b);
                  double proj[3];
                  double coeffs[3];
                  double d = ew::Geom3::nearest_on_polytope(proj, coeffs, q,
                   tri, 3);
                  if (d > 1e-10) {
                    continue;
                  }
                  int found = false;
                  for (int k = 0; k < n; k += 1) {
                    double s[3];
                    ew::Geom3::subtract(s, q, outp + 3 * k);
                    if (ew::Geom3::norm(s) < 1e-10) {
                      found = true;
                      break;
                    }
                  }
                  if (!found) {
                    report_error("missing face - box edge intersection");
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

const char *
ewtst::TestGeom3::test08(bool return_name)
{
  if (return_name) {
    return "crop_polytope timing";
  }
  double tri[900];
  for (int i = 0; i < 900; i += 1) {
    tri[i] = random_double();
  }
  ew::Bbox3 bb[100];
  for (int j = 0; j < 100; j += 1) {
    bb[j].min[0] = random_double() * 0.4;
    bb[j].min[1] = random_double() * 0.4;
    bb[j].min[2] = random_double() * 0.4;
    bb[j].max[0] = random_double() * 0.4 + 0.6;
    bb[j].max[1] = random_double() * 0.4 + 0.6;
    bb[j].max[2] = random_double() * 0.4 + 0.6;
  }
  double outp[85 * 3];
  const ew::Debug &dbg = ew::Debug::instance();
  double t0 = ew::Time::elapsed();
  for (int i = 0; i < 900; i += 9) {
    for (int j = 0; j < 100; j += 1) {
      ew::Geom3::crop_polytope(outp, tri + i, 3, bb + j);
    }
  }
  if (dbg.get_active("timing")) {
    double t1 = ew::Time::elapsed();
    dbg.dprintf("%s crop_polytope %d in %.6f average %.9f", "timing", 10000,
     t1 - t0, (t1 - t0) / 10000);
  }
  return 0;
}

const char *
ewtst::TestGeom3::test(int n, bool return_name)
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
  }
  return 0;
}

ewtst::TestGeom3::TestGeom3() :
 Test(Name, Desc, DebugVal)
{
}

int
ewtst::TestGeom3::exec(int ac, char **av)
{
  return Tst.exec_base(ac, av);
}

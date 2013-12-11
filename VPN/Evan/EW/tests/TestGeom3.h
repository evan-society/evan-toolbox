#ifndef EWTST_TESTGEOM3_H_INCLUDED
#define EWTST_TESTGEOM3_H_INCLUDED

// wdkg 2008-2010

#include "Test.h"

namespace ewtst {
  class TestGeom3 : public ewtst::Test {
  public:
    TestGeom3();
    static int exec(int ac, char **av);
  private:
    virtual const char *test(int n, bool return_name);
    void chk_nearest_tri_pt(double *tri, double *pt, bool known_coeffs,
     double *kc);
    void chk_nearest_tri(double *tri, bool regular);
    const char *test01(bool return_name);
    const char *test02(bool return_name);
    const char *test03(bool return_name);
    const char *test04(bool return_name);
    const char *test05(bool return_name);
    const char *test06(bool return_name);
    const char *test07(bool return_name);
    const char *test08(bool return_name);
  };
}

#endif

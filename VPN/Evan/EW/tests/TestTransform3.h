#ifndef EWTST_TESTTRANSFORM3_H_INCLUDED
#define EWTST_TESTTRANSFORM3_H_INCLUDED

// wdkg 2008-2010

#include "ew/Transform3.h"
#include "Test.h"

namespace ewtst {
  class TestTransform3 : public ewtst::Test {
  public:
    TestTransform3();
    static int exec(int ac, char **av);
  private:
    virtual const char *test(int n, bool return_name);
    void random_transform(ew::Transform3 *t);
    void check_apply(const ew::Transform3 *tr, double *d, const double *de);
    bool transforms_differ(const ew::Transform3 *tr, const ew::Transform3 *te)
     const;
    double determinant(const ew::Transform3 *tr) const;
    const char *test01(bool return_name);
    const char *test02(bool return_name);
    const char *test03(bool return_name);
    const char *test04(bool return_name);
    const char *test05(bool return_name);
    const char *test06(bool return_name);
    const char *test07(bool return_name);
    const char *test08(bool return_name);
    const char *test09(bool return_name);
    const char *test10(bool return_name);
    const char *test11(bool return_name);
    const char *test12(bool return_name);
  };
}

#endif

#ifndef EWTST_TESTTRANSFORM2_H_INCLUDED
#define EWTST_TESTTRANSFORM2_H_INCLUDED

// wdkg 2008-2010

#include "ew/Transform2.h"
#include "Test.h"

namespace ewtst {
  class TestTransform2 : public ewtst::Test {
  public:
    TestTransform2();
    static int exec(int ac, char **av);
  private:
    virtual const char *test(int n, bool return_name);
    void random_transform(ew::Transform2 *t);
    void check_apply(const ew::Transform2 *tr, double *d, const double *de);
    bool transforms_differ(const ew::Transform2 *tr, const ew::Transform2 *te)
     const;
    double determinant(const ew::Transform2 *tr) const;
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

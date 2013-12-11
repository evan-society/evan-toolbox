#ifndef EWTST_TESTSTRING_H_INCLUDED
#define EWTST_TESTSTRING_H_INCLUDED

// wdkg 2008-2010

#include "Test.h"

namespace ewtst {
  class TestString : public ewtst::Test {
  public:
    TestString();
    static int exec(int ac, char **av);
  private:
    virtual const char *test(int n, bool return_name);
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

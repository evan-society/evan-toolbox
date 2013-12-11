#ifndef EWTST_TESTDATAFLOW_H_INCLUDED
#define EWTST_TESTDATAFLOW_H_INCLUDED

// wdkg 2010

#include "Test.h"

namespace ewtst {
  class TestDataflow : public ewtst::Test {
  public:
    TestDataflow();
    static int exec(int ac, char **av);
  private:
    virtual const char *test(int n, bool return_name);
    const char *test01(bool return_name);
  };
}

#endif

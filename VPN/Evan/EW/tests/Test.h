#ifndef EWTST_TEST_H_INCLUDED
#define EWTST_TEST_H_INCLUDED

// wdkg 2009-2010

#include <string>
#include <stdint.h>

namespace ewtst {
  class Test {
  protected:
    Test(const char *i_name, const char *i_desc, const char *i_debug_val);
    uint32_t random_int();
    double random_double();
    std::string full_path(const char *tail);
    void report_error(const char *format, ...);
    int exec_base(int ac, char **av);
  private:
    virtual const char *test(int n, bool return_name) = 0;
    void random_seed(int v);
    const char *const name;
    const char *const desc;
    const char *const debug_val;
    uint32_t seed;
    int errs;
  };
}

#endif

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <string>
#include <stdexcept>
#include "ew/DataflowNetwork.h"
#include "ew/DataflowForm3.h"
#include "ew/ErrorIO.h"
#include "testlowercase.h"

namespace {
  const int BUFN = 100000;
}

static void
MyDiff(const char *msg, const char *file1, const char *file2,
 bool expdiff = false)
{
  char buf1[BUFN], buf2[BUFN];
  FILE *fd;
  std::string name1 = FullPath(file1);
  std::string name2 = FullPath(file2);
  fd = std::fopen(name1.c_str(), "rb");
  if (fd == 0) {
    throw std::runtime_error("fopen file1");
  }
  int n1 = std::fread(buf1, 1, BUFN, fd);
  if (std::ferror(fd) || std::fclose(fd) || n1 <= 0) {
    throw std::runtime_error("fread file1");
  }
  fd = std::fopen(name2.c_str(), "rb");
  if (fd == 0) {
    throw std::runtime_error("fopen file2");
  }
  int n2 = std::fread(buf2, 1, BUFN, fd);
  if (std::ferror(fd) || std::fclose(fd) || n2 <= 0) {
    throw std::runtime_error("fread file2");
  }
  if (n1 == BUFN || n2 == BUFN) {
    throw std::runtime_error("BUFN overflow");
  }
  if (expdiff) {
    if (n1 == n2 && std::memcmp(buf1, buf2, n1) == 0) {
      Error("%s", msg);
    }
  } else {
    if (n1 != n2 || std::memcmp(buf1, buf2, n1) != 0) {
      Error("%s", msg);
    }
  }
}

static const char Name01[] = "read/write";

static void
Test01()
{
  ew::DataflowNetwork nw;
  ew::DataflowForm3 *node = new ew::DataflowForm3(&nw);
  ew::Form3 form;
  std::string inpname = FullPath("test_form3_01.form");
  form.read_file(inpname.c_str());
  node->set_data(&form);
  std::string outpname = FullPath("test_form3.out");
  node->get_data()->write_file(outpname.c_str(), 0);
  node->decr_ref_count();
  MyDiff("unexpected output", "test_form3_01.form", "test_form3.out");
}

static const char Name02[] = "null read/write";

static void
Test02()
{
  ew::Form3 form;
  std::string inpname = FullPath("test_form3_02.form");
  form.read_file(inpname.c_str());
  std::string outpname = FullPath("test_form3.out");
  form.write_file(outpname.c_str(), 0);
  MyDiff("unexpected output", "test_form3_02.form", "test_form3.out");
}

static const char Name03[] = "null write";

static void
Test03()
{
  ew::Form3 form;
  std::string outpname = FullPath("test_form3.out");
  form.write_file(outpname.c_str(), 0);
  MyDiff("unexpected output", "test_form3_02.form", "test_form3.out");
}

static const char Name04[] = "reset";

static void
Test04()
{
  ew::DataflowNetwork nw;
  ew::DataflowForm3 *node = new ew::DataflowForm3(&nw);
  ew::Form3 form;
  std::string inpname = FullPath("test_form3_01.form");
  form.read_file(inpname.c_str());
  node->set_data(&form);
  node->reset();
  std::string outpname = FullPath("test_form3.out");
  node->get_data()->write_file(outpname.c_str(), 0);
  node->decr_ref_count();
  MyDiff("unexpected output", "test_form3_02.form", "test_form3.out");
}

static const char Name05[] = "repeated read";

static void
Test05()
{
  ew::DataflowNetwork nw;
  ew::DataflowForm3 *node = new ew::DataflowForm3(&nw);
  ew::DataflowForm3 *node2 = new ew::DataflowForm3(&nw);
  for (unsigned long i = 0; i < 1000; i += 1) {
    if (node->get_version() != i + 1 ||
     node->get_change_cycle_coords() != i + 1 ||
     node->get_change_cycle_relax() != i + 1 ||
     node->get_change_cycle_association() != i + 1) {
      Error("bad version");
    }
    node->get_version();
    ew::Form3 form;
    std::string inpname = FullPath("test_form3_01.form");
    form.read_file(inpname.c_str());
    node->set_data(&form);
    inpname = FullPath("test_form3_02.form");
    form.read_file(inpname.c_str());
    node2->set_data(&form);
  }
  node->decr_ref_count();
  node2->decr_ref_count();
}

static const char Name06[] = "repeated create and read";

static void
Test06()
{
  ew::DataflowNetwork nw;
  for (unsigned long i = 0; i < 1000; i += 1) {
    ew::DataflowForm3 *node = new ew::DataflowForm3(&nw);
    ew::Form3 form;
    std::string inpname = FullPath("test_form3_01.form");
    form.read_file(inpname.c_str());
    node->set_data(&form);
    node->decr_ref_count();
  }
}

static const char Name07[] = "missing file";

static void
Test07()
{
  std::string s;
  std::string inpname = FullPath("test_form3_00.form");
  try {
    ew::Form3 form;
    form.read_file(inpname.c_str());
  } catch (ew::ErrorIO &e) {
    s = e.what();
    std::string exp = "The ew_form3 file ";
    exp += inpname;
    exp += " could not be loaded (the file could not be opened or does not"
     " contain valid xml).";
    if (strcmp(e.what(), exp.c_str()) == 0) {
      return;
    }
  } catch (...) {
  }
  Error("wrong or no error %s", s.c_str());
}

static const char Name08[] = "not xml";

static void
Test08()
{
  std::string s;
  std::string inpname = FullPath("test_form3_03.form");
  try {
    ew::Form3 form;
    form.read_file(inpname.c_str());
  } catch (ew::ErrorIO &e) {
    s = e.what();
    std::string exp = "The ew_form3 file ";
    exp += inpname;
    exp += " could not be loaded (the file could not be opened or does not"
     " contain valid xml).";
    if (strcmp(e.what(), exp.c_str()) == 0) {
      return;
    }
  } catch (...) {
  }
  Error("wrong or no error %s", s.c_str());
}

static const char Name09[] = "not ew_form3";

static void
Test09()
{
  std::string s;
  std::string inpname = FullPath("test_form3_04.form");
  try {
    ew::Form3 form;
    form.read_file(inpname.c_str());
  } catch (ew::ErrorIO &e) {
    s = e.what();
    std::string exp = "The ew_form3 file ";
    exp += inpname;
    exp += " could not be loaded (invalid format, wrong incorrect document"
     " type in line 2).";
    if (strcmp(e.what(), exp.c_str()) == 0) {
      return;
    }
  } catch (...) {
  }
  Error("wrong or no error %s", s.c_str());
}

static const char Name10[] = "bad content";

static void
Test10()
{
  std::string inpname = FullPath("test_form3_07.form");
  try {
    ew::Form3 form;
    form.read_file(inpname.c_str());
  } catch (ew::ErrorIO &e) {
    std::string exp = "The ew_form3 file ";
    exp += inpname;
    exp += " could not be loaded (invalid format, bad locations value in"
     " line 5).";
    if (strcmp(e.what(), exp.c_str()) == 0) {
      return;
    }
    Error("wrong error: %s", e.what());
    return;
  } catch (...) {
    Error("wrong error");
    return;
  }
  Error("no error");
}

static const char Name11[] = "comments and unknown records";

static void
Test11()
{
  ew::Form3 form;
  std::string inpname = FullPath("test_form3_08.form");
  form.read_file(inpname.c_str());
  std::string outpname = FullPath("test_form3.out");
  form.write_file(outpname.c_str(), 0);
  MyDiff("unexpected output", "test_form3_02.form", "test_form3.out");
}

static const char Name12[] = "compressed output";

static void
Test12()
{
  ew::Form3 form, form2;
  std::string inpname = FullPath("test_form3_02.form");
  form.read_file(inpname.c_str());
  std::string outpname = FullPath("test_form3.out");
  form.write_file(outpname.c_str(), 1);
  MyDiff("unexpected output", "test_form3_02.form", "test_form3.out", true);
  inpname = FullPath("test_form3.out");
  form2.read_file(inpname.c_str());
  if (!(form == form2)) {
    Error("forms different");
  }
}

static const char Name13[] = "compressed input";

static void
Test13()
{
  ew::Form3 form;
  std::string inpname = FullPath("test_form3_02.form.gz");
  form.read_file(inpname.c_str());
  std::string outpname = FullPath("test_form3.out");
  form.write_file(outpname.c_str(), 0);
  MyDiff("unexpected output", "test_form3_02.form", "test_form3.out");
}

static test_t Tests[] = {
  {Name01, &Test01},
  {Name02, &Test02},
  {Name03, &Test03},
  {Name04, &Test04},
  {Name05, &Test05},
  {Name06, &Test06},
  {Name07, &Test07},
  {Name08, &Test08},
  {Name09, &Test09},
  {Name10, &Test10},
  {Name11, &Test11},
  {Name12, &Test12},
  {Name13, &Test13},
  {0, 0}
};

//XXX manual changes/write/get_version()s
//XXX check values after input, check fp .39999999999999997, utf8

int
main(int ac, char **av)
{
  return ProcessCmdline("test_form3",
   "Exercise the dataflow form3 node.\n"
   Tests, ac, av);
}

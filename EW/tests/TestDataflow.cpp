// wdkg 2010

//XXX check notifier_alert set when supposed to
//XXX check debug output
//XXX check reference counting
//XXX exhaustive test of ew/DataflowNetwork.h
//XXX exhaustive test of ew/DataflowNode.h

#include "ew/DataflowNetwork.h"
#include "Test.h"
#include "DataflowTestNode.h"
#include "DataflowTestNotifier.h"
#include "TestDataflow.h"

namespace {
  const char Name[] = "test_dataflow";
  const char Desc[] = "Exercise ew::DataflowNetwork and ew::DataflowNode.\n";
  const char *DebugVal = 0;
// Static variable.
  ewtst::TestDataflow Tst;
}

const char *
ewtst::TestDataflow::test01(bool return_name)
{
  if (return_name) {
    return "blind run through";
  }
  ew::DataflowNetwork nw;
  DataflowTestNotifier *nt = new DataflowTestNotifier(&nw);
  nt->notifier_register();
  ewtst::DataflowTestNode *t1 = new ewtst::DataflowTestNode(&nw);
  ewtst::DataflowTestNode *t2 = new ewtst::DataflowTestNode(&nw);
  t1->set_n(-1);
  t1->get_version();
  if (nt->notifier_alert) {
    nt->notifier_alert = false;
    nt->notifier_register();
  }
  t2->set_n(-1);
  t2->get_version();
  if (nt->notifier_alert) {
    nt->notifier_alert = false;
    nt->notifier_register();
  }
  t1->set_dep1(t2);
  t1->set_n(1);
  t2->set_n(2);
  t1->get_version();
  t2->set_n(5);
  t2->decr_ref_count();
  t1->decr_ref_count();
  return 0;
}

const char *
ewtst::TestDataflow::test(int n, bool return_name)
{
  switch(n) {
  case 1:
    return test01(return_name);
  }
  return 0;
}

ewtst::TestDataflow::TestDataflow() :
 Test(Name, Desc, DebugVal)
{
}

int
ewtst::TestDataflow::exec(int ac, char **av)
{
  return Tst.exec_base(ac, av);
}

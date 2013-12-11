#ifndef EWTSTS_DATAFLOWTESTNOTIFIER_H_INCLUDED
#define EWTSTS_DATAFLOWTESTNOTIFIER_H_INCLUDED

// wdkg 2008-2010

#include "ew/DataflowNetwork.h"
#include "ew/DataflowNotifier.h"

namespace ewtst {
  class DataflowTestNotifier : public ew::DataflowNotifier {
  public:
    inline DataflowTestNotifier(ew::DataflowNetwork *nw);
    bool notifier_alert;
  private:
    inline virtual void notifier_handle_cb();
  };
}

inline
ewtst::DataflowTestNotifier::DataflowTestNotifier(ew::DataflowNetwork *nw) :
 ew::DataflowNotifier(nw),
 notifier_alert(false)
{
}

inline void
ewtst::DataflowTestNotifier::notifier_handle_cb()
{
  notifier_alert = true;
}

#endif

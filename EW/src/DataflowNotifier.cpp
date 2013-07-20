// wdkg 2008-2010

#include "ew/DataflowNetwork.h"
#include "ew/DataflowNotifier.h"

ew::DataflowNotifier::DataflowNotifier(ew::DataflowNetwork *i_network) :
// next_notifier uninitialized
 network(i_network),
 next_notifier(NULL),
 is_registered(false)
{

}

ew::DataflowNotifier::~DataflowNotifier()
{
  notifier_unregister();
}

void
ew::DataflowNotifier::notifier_unregister()
{
  if (!is_registered) {
    return;
  }
  is_registered = false;
  if (network->first_notifier == this) {
    network->first_notifier = next_notifier;
    if (network->last_notifier == this) {
      network->last_notifier = 0;
    }
  } else {
    for (ew::DataflowNotifier *n = network->first_notifier; n != 0;
     n = n->next_notifier) {
      if (n->next_notifier == this) {
        n->next_notifier = next_notifier;
        if (next_notifier == 0) {
          network->last_notifier = n;
        }
        break;
      }
    }
  }
}

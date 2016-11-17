#ifndef EW_DATAFLOWNOTIFIER_H_INCLUDED
#define EW_DATAFLOWNOTIFIER_H_INCLUDED

// wdkg 2008-2010

//XXX doc as a way of tracing network
// Class ew::DataflowNotifier is a utility class to provide a callback when
// anything changes in the network.
// Inherit this class, implement callback notifier_handle_cb() and call
// register() to get notified when any changes are made
// to the dataflow network.
// The notifier is intended for use when flow is controlled by an event loop.
// The handler should do nothing more than set a flag or schedule later
// processing with an idle handler.
// Typically notifier_handle_cb() will call an idle handler.
// For efficiency, it gets called before changes are instantiated in the
// network.
// The notifier is automatically unregistered when triggered.
// It should not re-register itself, but its idle handler can.

// This is designed to be privately inherited, so redundent member names are
// used to make inadvertent name clashes less likely.

#include "ew/DataflowNetwork.h"
#include "ew/DataflowNotifier.h"

// ew::DataflowNetwork accesses all private members.

namespace ew {
  class DataflowNotifier {
    friend class DataflowNetwork;
  public:
    explicit DataflowNotifier(ew::DataflowNetwork *i_network);
    virtual ~DataflowNotifier();
    inline bool notifier_is_registered() const;
    inline void notifier_register();
    void notifier_unregister();
    ew::DataflowNetwork *const network;
  private:
    virtual void notifier_handle_cb() = 0;
    ew::DataflowNotifier *next_notifier;
    bool is_registered;
// undefined
    DataflowNotifier(const ew::DataflowNotifier &that);
    DataflowNotifier &operator=(const ew::DataflowNotifier &that);
  };
}

inline bool
ew::DataflowNotifier::notifier_is_registered() const
{
  return is_registered;
}

inline void
ew::DataflowNotifier::notifier_register()
{
  if (is_registered) {
    return;
  }
  is_registered = true;
  next_notifier = 0;
  if (network->first_notifier == 0) {
    network->first_notifier = network->last_notifier = this;
  } else {
// first_notifier and last_notifier are only pretending to be
// ew::Dataflow_notifier *'s.
    ew::DataflowNotifier *n = network->last_notifier;
    n->next_notifier = this;
    network->last_notifier = this;
  }
}

#endif

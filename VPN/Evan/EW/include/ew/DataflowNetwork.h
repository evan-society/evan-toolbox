#ifndef EW_DATAFLOWNETWORK_H_INCLUDED
#define EW_DATAFLOWNETWORK_H_INCLUDED

// wdkg 2008-2010

#include <string>
#include <vector>
#include "ew/DebugHelper.h"
#include "ew/FriendVariable.h"

//XXX method to free unreferenced cache elements
//XXX  possibly limit number of cached surfaces
//XXX support reloading of changed files in cache
//XXX document start_changing and start_updating
//XXX  void uncache_surface(const ew::DataflowSurface3E *sur);

// The counter could conceivably overflow, resulting in a runtime exception
// when a change is made to the network.
// It could be an unsigned long long if this ever actually happened, but
// most other occurences of "unsigned long" would have to be changed to,
// and the % specifiers in ew::Debug::dprintf's.

namespace ew {
// XXX forward declaration w/o include
  class DataflowNode;
// XXX forward declaration w/o include
  class DataflowNotifier;
// XXX forward declaration w/o include
  class DataflowSurface3E;
// XXX forward declaration w/o include
  class DataflowCurve3E;
// XXX forward declaration w/o include
  class View3Widget;

/// @brief Lightweight Dataflow Network
/// @headerfile ew/DataflowNetwork.h "ew/DataflowNetwork.h"
///
/// EW::DataflowNetwork provides the infrastructure for a lightweight dataflow
/// network.
///
/// To keep track of what calculations done by nodes in the network are up to
/// date, and which need to be redone, EW::DataflowNetwork keeps a cycle
/// counter.
/// A cycle consists of an initial phase, when changes are made to nodes of the
/// network, followed by a second phase, when updates of various calculations
/// are made.
/// Requesting updated output from a node puts the network into the update
/// phase of the same cycle, if it is not already in an update phase.
/// Changing a node puts the network into the change phase of the next cycle,
/// if it is not already in a change phase.
/// Initially the network is in the change phase of cycle 1.
///
/// Operations on a network must not be performed simultaneously in different
/// threads.
/// However, operations on different networks may be.
///
/// This class does not support assignment or comparison.
/// It contains private members and undocumented members.
  class DataflowNetwork {
  public:
/// This creates an empty
/// network.
    DataflowNetwork();
/// This destroys the network.
/// All nodes and other objects using this network must already have been
/// destroyed.
    ~DataflowNetwork();
    inline void start_changing();
    inline void start_updating();
/// @return
/// The current cycle number.
    inline unsigned long get_cycle() const;
/// The first time this is called with a given filename, it reads the curve
/// from the file, creates a new explicit curve node using it, and returns
/// a pointer to it.
/// Subsequently it increments the same node's reference count and returns a
/// pointer to it.
/// When the pointer to the node is no longer needed, the reference should be
/// released with EW::DataflowNode::decr_ref_count.
/// The cache retains a reference to the node, so it will not be deleted until
/// all other references are released and the network is destroyed.
/// @param filename is the canonical name of the file to read.
/// @exception std::bad_alloc
/// @exception EW::ErrorIO
    const ew::DataflowCurve3E *cached_curve(const char *filename);
/// The first time this is called with a given filename, it reads the surface
/// from the file, creates a new explicit surface node using it, and returns
/// a pointer to it.
/// Subsequently it increments the same node's reference count and returns a
/// pointer to it.
/// When the pointer to the node is no longer needed, the reference should be
/// released with EW::DataflowNode::decr_ref_count.
/// The cache retains a reference to the node, so it will not be deleted until
/// all other references are released and the network is destroyed.
/// @param filename
///   The name of the file to
///   read.
/// @exception std::bad_alloc
/// @exception EW::ErrorIO
    const ew::DataflowSurface3E *cached_surface(const char *filename);
// undocumented
    const ew::DebugHelper dbg;
// private to the library
    ew::FriendVariable<int, ew::DataflowNode> dbg_last_node_n;
    ew::FriendVariable<int, ew::View3Widget> dbg_last_widget_n;
    ew::FriendVariable<ew::DataflowNotifier *, ew::DataflowNetwork,
     ew::DataflowNotifier> first_notifier;
    ew::FriendVariable<ew::DataflowNotifier *, ew::DataflowNetwork,
     ew::DataflowNotifier> last_notifier;
  private:
    void start_changing_helper();
    void start_updating_helper();
    unsigned long cycle;
    bool is_updating;
    std::vector<ew::DataflowCurve3E *> cached_curves;
    std::vector<std::string> cached_curve_names;
    std::vector<ew::DataflowSurface3E *> cached_surfaces;
    std::vector<std::string> cached_surface_names;
// undefined
    DataflowNetwork(const ew::DataflowNetwork &that);
    DataflowNetwork &operator=(const ew::DataflowNetwork &that);
  };

}

inline unsigned long
ew::DataflowNetwork::get_cycle() const
{
  return cycle;
}

inline void
ew::DataflowNetwork::start_changing()
{
  if (is_updating || first_notifier != 0) {
    start_changing_helper();
  }
}

inline void
ew::DataflowNetwork::start_updating()
{
  if (!is_updating) {
    start_updating_helper();
  }
}

#endif

#ifndef EW_DATAFLOWNODE_H_INCLUDED
#define EW_DATAFLOWNODE_H_INCLUDED

// wdkg 2008-2010

//XXX abstract ew::DataflowPoints, put get_bbox here as well as warping
// stuff.

#include <stdexcept>
#include <string>
#include <vector>
#include "ew/DebugHelper.h"
#include "ew/DataflowNetwork.h"

namespace ew {
/// @brief Node Base Class
///
/// @headerfile ew/DataflowNode.h "ew/DataflowNode.h"
/// EW::DataflowNode is the base class for nodes in a EW::DataflowNetwork.
///
/// EW::DataflowNode does not support assignment or comparison.
/// It contains private members, protected members and undocumented members.
///
/// Nodes are referenced counted and can only be deleted indirectly by
/// calling #decr_ref_count.
/// The reference count is set to 1 by the constructor.
///
/// Depending on its parameters, a node is considered valid or invalid.
/// What constitutes a valid state depends on the specific node class.
///
/// A node can depend on other nodes.
/// The possible dependencies are defined by the specific node class.
/// It is a precondition of methods that set these dependencies that they do
/// not create dependency loops.
///
/// After the EW::DataflowNetwork has been destroyed, the only
/// EW::DataflowNode method that may be called is decr_ref_count.
  class DataflowNode {
  public:
/// This resets a node to the state it had immediately after
/// construction.
    virtual void reset() = 0;
/// This increases the reference
/// count.
    void incr_ref_count() const;
/// This decreases the reference count.
/// If the reference count becomes zero, the node is deleted.
    void decr_ref_count() const;
/// @return
///   @c The last cycle on which a change to the node was made.
    inline unsigned long get_change_cycle() const;
/// The version of a node is the greater of the last network cycle that any
/// parameter of the node was changed, and the versions of any nodes that the
/// node depends on.
/// This will force the network into an updating phase.
/// @return
///   @c The version of the node.
    inline unsigned long get_version() const;
/// @return
///   @c true if the node is in a valid state.
/// @post The network will be in an update phase.
    inline bool get_valid() const;
/// This points to the EW::Dataflow_Network that this EW::Dataflow_Node was
/// created with.
    ew::DataflowNetwork *const network;
    const char *const *const dbg_dependency_names;
    const int dbg_n;
    const ew::DebugHelper dbg;
    static const char *to_str(const ew::DataflowNode *n);
  protected:
    DataflowNode(ew::DataflowNetwork *i_network, int ndeps,
     const char *dbg_final_name,
     const char *const *const i_dbg_dependency_names);
    virtual ~DataflowNode();
    void set_dependency(int i, const ew::DataflowNode *dep);
    inline void update() const;
    inline void record_change();
    unsigned long change_cycle;
    std::vector<const ew::DataflowNode *> inp_dependencies;
    std::vector<unsigned long> change_cycle_dependencies;
    mutable bool outp_valid;
    mutable unsigned long outp_version;
    mutable unsigned long update_cycle;
  private:
    int next_dbg_n();
    virtual void update_helper() const = 0;
    bool depends_on(ew::DataflowNode *node) const;
    mutable int ref_count;
// undefined
    DataflowNode(const ew::DataflowNode &that);
    DataflowNode &operator=(const ew::DataflowNode &that);
  };
}


// Concrete node classes must implement Ctor, reset and update_helper.

// Any exception thrown by idle callbacks will be passed through here.
// It is permissible, and recommended, to call this before changes are made.
// That way if it throws an exception the network is left in a consistent
// state.

inline void
ew::DataflowNode::record_change()
{
  network->start_changing();
  change_cycle = network->get_cycle();
}

inline void
ew::DataflowNode::update() const
{
  if (update_cycle != network->get_cycle()) {
    network->start_updating();
    update_helper();
    update_cycle = network->get_cycle();
  }
}

inline unsigned long
ew::DataflowNode::get_change_cycle() const
{
  return change_cycle;
}

inline unsigned long
ew::DataflowNode::get_version() const
{
  update();
  return outp_version;
}

inline bool
ew::DataflowNode::get_valid() const
{
  update();
  return outp_valid;
}

#endif

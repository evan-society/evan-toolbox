#ifndef EWTST_DATAFLOWTESTNODE_H_INCLUDED
#define EWTST_DATAFLOWTESTNODE_H_INCLUDED

// wdkg 2010

#include <stdexcept>
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DebugHelper.h"

namespace ewtst {
  class DataflowTestNode : public ew::DataflowNode {
  public:
    explicit DataflowTestNode(ew::DataflowNetwork *i_network);
    void set_n(int i);
    inline int get_n() const;
    void set_dep1(const ewtst::DataflowTestNode *dep);
    inline const ewtst::DataflowTestNode* get_dep1() const;
    inline unsigned long get_change_cycle_dep1() const;
    void set_dep2(const ewtst::DataflowTestNode *dep);
    inline const ewtst::DataflowTestNode* get_dep2() const;
    inline unsigned long get_change_cycle_dep2() const;
    virtual void reset();
    inline int get_sum() const;
    const ew::DebugHelper dbg;
  private:
    virtual void update_helper() const;
    int inp_n;
    mutable int outp_sum;
  };
}

inline int
ewtst::DataflowTestNode::get_n() const
{
  return inp_n;
}

inline const ewtst::DataflowTestNode *
ewtst::DataflowTestNode::get_dep1() const
{
  return static_cast<const ewtst::DataflowTestNode *>(inp_dependencies[0]);
}

inline unsigned long
ewtst::DataflowTestNode::get_change_cycle_dep1() const
{
  return ew::DataflowNode::change_cycle_dependencies[0];
}

inline const ewtst::DataflowTestNode *
ewtst::DataflowTestNode::get_dep2() const
{
  return static_cast<const ewtst::DataflowTestNode *>(inp_dependencies[1]);
}

inline unsigned long
ewtst::DataflowTestNode::get_change_cycle_dep2() const
{
  return ew::DataflowNode::change_cycle_dependencies[1];
}

inline int
ewtst::DataflowTestNode::get_sum() const
{
// This calls update().
  if (!ew::DataflowNode::get_valid()) {
    throw std::runtime_error("invalid node");
  }
  return outp_sum;
}

#endif

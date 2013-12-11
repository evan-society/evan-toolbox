#ifndef EW_DATAFLOWIMAGE3_H_INCLUDED
#define EW_DATAFLOWIMAGE3_H_INCLUDED

// wdkg 2008-2010

#include "ew/DebugHelper.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowArray3.h"
#include "ew/DataflowData3.h"

namespace ew {

  class DataflowImage3 : public ew::DataflowNode {
  public:
    explicit DataflowImage3(ew::DataflowNetwork *i_network);
    virtual void reset();
    void set_array_node(const ew::DataflowArray3 *arr);
    inline const ew::DataflowArray3 *get_array_node() const;
    inline unsigned long get_change_cycle_array_node() const;
    void set_data_node(const ew::DataflowData3 *dat);
    inline const ew::DataflowData3 *get_data_node() const;
    inline unsigned long get_change_cycle_data_node() const;
    const ew::DebugHelper dbg;
  private:
    virtual void update_helper() const;
  };

}

inline const ew::DataflowArray3 *
ew::DataflowImage3::get_array_node() const
{
  return static_cast<const ew::DataflowArray3 *>(
   ew::DataflowNode::inp_dependencies[0]);
}

inline const ew::DataflowData3 *
ew::DataflowImage3::get_data_node() const
{
  return static_cast<const ew::DataflowData3 *>(
   ew::DataflowNode::inp_dependencies[1]);
}

inline unsigned long
ew::DataflowImage3::get_change_cycle_array_node() const
{
  return ew::DataflowNode::change_cycle_dependencies[0];
}

inline unsigned long
ew::DataflowImage3::get_change_cycle_data_node() const
{
  return ew::DataflowNode::change_cycle_dependencies[1];
}

#endif

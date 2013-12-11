#ifndef EW_DATAFLOWDATA3_H_INCLUDED
#define EW_DATAFLOWDATA3_H_INCLUDED

// wdkg 2008-2010

// This class is implemented entirely in this header.

#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"

namespace ew {

  class DataflowData3 : public ew::DataflowNode {
  public:
    inline const int *get_size() const;
  private:
    DataflowData3(ew::DataflowNetwork *i_network, int ndeps,
     const char *dbg_final_name,
     const char *const *const i_dbg_dependency_names);
    mutable int outp_size[3];
  };

}

inline
ew::DataflowData3::DataflowData3(ew::DataflowNetwork *i_network, int ndeps,
 const char *dbg_final_name, const char *const *const i_dbg_dependency_names) :
 ew::DataflowNode(i_network, ndeps, dbg_final_name, i_dbg_dependency_names)
{
}

inline const int *
ew::DataflowData3::get_size() const
{
  return outp_size;
}

#endif

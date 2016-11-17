#ifndef EW_INDEX3TOINDEX_H_INCLUDED
#define EW_INDEX3TOINDEX_H_INCLUDED

// wdkg 2010

// This class is implemented entirely in this header.
// XXX replace by struct

#include <vector>

namespace ew {

  class Index3ToIndex {
  public:
    int node;
    std::vector<int> polytopes;
    int offsets[3];
  };

}

#endif

#ifndef EW_INDEX3TOCHECK_H_INCLUDED
#define EW_INDEX3TOCHECK_H_INCLUDED

// wdkg 2010

// This class is implemented entirely in this header.
// XXX replace by struct

namespace ew {

  class Index3ToCheck {
  public:
    unsigned int node;
    int subdivide[3];
    int offsets[3];
    int dir;
  };

}

#endif

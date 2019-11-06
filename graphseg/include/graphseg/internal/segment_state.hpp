#ifndef GRAPHSEG_CPP_GRAPHSEG_SEGMENT_STATE_HPP
#define GRAPHSEG_CPP_GRAPHSEG_SEGMENT_STATE_HPP

namespace GraphSeg 
{
  enum class SegmentStatus
  {
    NONE,
    INITIALIZED,
    MERGED,
    SMALLED,
    TERMINATED
  };
}

#endif
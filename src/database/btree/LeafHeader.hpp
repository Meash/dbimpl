//
// Created by Martin on 31.05.2015.
//

#ifndef PROJECT_LEAFHEADER_HPP
#define PROJECT_LEAFHEADER_HPP

#include <stddef.h>

struct LeafHeader {
  static const uint64_t INVALID_PAGE_ID = (uint64_t) (int64_t) -1; // max value

  size_t keyCount;
  uint64_t previousLeafPageId;
  uint64_t nextLeafPageId;

  LeafHeader(uint64_t previous, uint64_t next)
      : previousLeafPageId(previous), nextLeafPageId(next),
        keyCount(0) { }
};

#endif //PROJECT_LEAFHEADER_HPP

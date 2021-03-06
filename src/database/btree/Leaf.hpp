//
// Created by Martin on 31.05.2015.
//

#ifndef PROJECT_LEAF_H
#define PROJECT_LEAF_H

#include <stddef.h>
#include <stdint.h>
#include "Entry.hpp"
#include "BTreeConstants.hpp"
#include "../slotted_pages/TID.hpp"
#include "LeafHeader.hpp"

template<typename KeyType, typename KeyComparator>
struct Leaf {
public:
  LeafHeader header;

  Entry<KeyType, TID> entries[
      BTreeConstants<KeyType>::maxLeafCapacity];

  Leaf(uint64_t previousPageId, uint64_t nextPageId)
      : header(previousPageId, nextPageId) { }

  inline bool hasSpaceForOneMoreEntry();

  inline void insertDefiniteFit(KeyType key, TID tid, KeyComparator &smaller);

  inline void erase(KeyType key, KeyComparator &smaller);

  int getEntriesRightBound();

  int getEntriesLeftBound();

  bool lookup(KeyType key, KeyComparator &smaller, TID *resultTid);
};

#include "Leaf.inl.cpp"

#endif //PROJECT_LEAF_H

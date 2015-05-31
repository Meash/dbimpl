//
// Created by Martin on 31.05.2015.
//

#ifndef PROJECT_CONSTS_HPP
#define PROJECT_CONSTS_HPP

#include "../buffer/BufferManager.hpp"
#include "../slotted_pages/TID.hpp"

template<class KeyType, class KeyComparator>
class BTreeConstants {
public:
  static const uint64_t maxNodeCapacity = (BufferManager::DATA_SIZE_BYTE
                                           - sizeof(InnerNode<KeyType, KeyComparator>::header))
                                          / sizeof(Entry<KeyType, uint64_t>)
                                          - 1 /* one less for the additional K/V pair */;
  static const uint64_t minNodeCapacity = maxNodeCapacity / 2;
  static const uint64_t maxLeafCapacity = (BufferManager::DATA_SIZE_BYTE
                                           - sizeof(InnerNode<KeyType, KeyComparator>::header))
                                          / sizeof(Entry<KeyType, TID>)
                                          - 1 /* one less for the additional K/V pair */;
  static const uint64_t minLeafCapacity = maxLeafCapacity / 2;
};

#endif //PROJECT_CONSTS_HPP

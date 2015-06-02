//
// Created by daniel on 5/19/15.
//

#ifndef PROJECT_BTREE_H
#define PROJECT_BTREE_H

#include <stdint.h>
#include <vector>
#include "Entry.hpp"
#include "InnerNode.hpp"
#include "Leaf.hpp"
#include "../buffer/BufferManager.hpp"
#include "../slotted_pages/TID.hpp"

template<typename KeyType, typename KeyComparator>
struct FrameNode {
  BufferFrame *frame;
  InnerNode<KeyType, KeyComparator> *node;
};
template<typename KeyType, typename KeyComparator>
struct FrameLeaf {
  BufferFrame *frame;
  Leaf<KeyType, KeyComparator> *leaf;
};

/**
 * Entries less than the key are on the left, entries greater than or equal to the key are on the right.
 *
 * The first key in the entries array of InnerNodes does not have a meaning,
 * it only fills up the array from the left with the additional necessary value.
 *
 * Splits are always done to the right; the new node/leaf will be to the right of the old one.
 *
 * Merges are never performed. We assume a realistic database where inserts heavily outweigh the deletes
 * (should there even be any deletes at all).
 *
 * Invariants:
 * 1) the parent node of an inner node in an insert call always has space for at least one more entry
 * 2) the parent node of a leaf in an insert call always has space for at least one more entry
 */
template<typename KeyType, typename KeyComparator>
class BTree {
private:
  BufferManager &bufferManager;
  uint64_t segmentId;
  KeyComparator smallerComparator;

  uint64_t rootPageId : 48;
  size_t treeSize; // number of elements inside of the tree
  /**
 * Longest path from the root to one of the leafs.
 * I.e., a tree with only the root leaf has a height of 0.
 */
  size_t height;

  uint64_t lastPageId;

  BufferFrame *findFrameForKey(KeyType key, bool exclusive);

  inline bool searchForKey(KeyType key, TID &tid, uint64_t pageId, size_t currentHeight);

  inline bool isLeafHeight(size_t height);

  inline FrameNode<KeyType, KeyComparator> splitInnerNode(
      InnerNode<KeyType, KeyComparator> *node,
      uint64_t nodePageId,
      InnerNode<KeyType, KeyComparator> *parent);

  /**
 * Returns the page id of the leaf that contains the key
 */
  inline FrameLeaf<KeyType, KeyComparator> splitLeaf
      (Leaf<KeyType, KeyComparator> *leaf, BufferFrame *leafFrame, uint64_t leafPageId,
       InnerNode<KeyType, KeyComparator> *parentNode,
       KeyType key);


  /**
 * returns the most left leaf of the tree
 */
  Leaf<KeyType, KeyComparator> getMostLeftLeaf();

  /**
 * returns the most right leaf of the tree
 */
  Leaf<KeyType, KeyComparator> getMostRightLeaf();

  inline uint64_t nextPageId();

  Leaf<KeyType, KeyComparator> &getLeaf(KeyType key);

  FrameNode<KeyType, KeyComparator> createEmptyNode(uint64_t pageId);

public:
  inline BTree(BufferManager &bManager, uint64_t segmentId, KeyComparator &smaller);

  inline void insert(KeyType key, TID tid);

  inline bool lookup(KeyType key, TID &tid);

  inline std::vector<TID> lookupRange(KeyType begin, KeyType end);

  inline void erase(KeyType key);

  inline uint64_t size();

  inline void visualize();

  inline void visualizeNode(InnerNode<KeyType, KeyComparator> *node, uint64_t *leafId, uint64_t *nodeId,
                            uint64_t curDepth, uint64_t maxDepth);

  inline void visualizeLeaf(Leaf<KeyType, KeyComparator> *leaf, uint64_t leafId);
};

#include "BTree.inl.cpp"
#include "BTreeVisualize.inl.cpp"

#endif //PROJECT_BTREE_H

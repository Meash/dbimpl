#ifndef PROJECT_CHAININGHT_H
#define PROJECT_CHAININGHT_H

#include "hash.hpp"
#include "inttypes_wrapper.hpp"
#include <atomic>
#include <stdio.h>
#include <string.h>

class ChainingHT {
public:
  // Chained tuple entry
  struct Entry {
    uint64_t key;
    Entry *next;

    Entry(uint64_t key) : key(key), next(nullptr) { }

    Entry() { }
  };

private:
  std::atomic<Entry *> *atomicEntries;
  uint64_t size;

public:
  // Constructor
  ChainingHT(uint64_t buildSize) : size(buildSize * 3 /* 48 max per entry / sizeof(Entry) = 48 / (8 + 8) */) {
    atomicEntries = new std::atomic<Entry *>[size];
    memset(atomicEntries, 0, (size_t) size * sizeof(std::atomic<Entry *>));
  }

  // Destructor
  ~ChainingHT() {
    delete[] atomicEntries;
  }

  // Returns the number of hits
  inline uint64_t lookup(uint64_t key) const {
    uint64_t hash = hashKey(key) % size;
    Entry *entry = atomicEntries[hash];
    uint64_t count = 0;
    while (entry != nullptr) {
      // branch-free. equivalent to if(entry->key == key) count++
      bool inc = entry->key == key;
      count += inc;
      entry = entry->next;
    }
    return count;
  }

  inline void insert(Entry *newEntry) {
    uint64_t hash = hashKey(newEntry->key) % size;

    Entry *oldEntry;
    do {
      oldEntry = atomicEntries[hash];
      newEntry->next = oldEntry;
    } while (!atomicEntries[hash]
        .compare_exchange_strong(oldEntry, newEntry,
                                 std::memory_order_seq_cst, std::memory_order_seq_cst));
  }

  inline void print() {
    for (unsigned i = 0; i < size; i++) {
      Entry *entry = atomicEntries[i];
      if (entry == nullptr) {
        continue;
      }
      printf("[%u]", i);
      while (entry != nullptr) {
        printf(" -> (%" PRIu64 ")", entry->key);
        entry = entry->next;
      }
      printf("\n");
    }
  }
};

#endif //PROJECT_CHAININGHT_H

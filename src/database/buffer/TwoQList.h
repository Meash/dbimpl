//
// Created by Daniel on 29.04.2015.
//

#ifndef PROJECT_TWOQLIST_H
#define PROJECT_TWOQLIST_H


#include "../buffer/BufferFrame.hpp"
#include "../buffer/IReplacementStrategy.h"
#include <stdint.h>
#include <set>
#include <list>

class TwoQList : public IReplacementStrategy{

    std::set<BufferFrame*> FifoSet;
    std::set<BufferFrame*> LruSet;
    std::list<BufferFrame*> FifoQueue;
    std::list<BufferFrame*> LruQueue;
public:
    void push(BufferFrame *frame);
    void remove(BufferFrame *frame);
    BufferFrame* pop();
private:

};


#endif //PROJECT_TWOQLIST_H

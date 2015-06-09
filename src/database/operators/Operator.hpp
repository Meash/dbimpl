//
// Created by daniel on 6/9/15.
//

#ifndef PROJECT_OPERATOR_HPP
#define PROJECT_OPERATOR_HPP

#include <vector>

class Operator {
public:
  virtual void open() = 0;

  virtual bool next() = 0;

  virtual std::vector<Register> getOutput() = 0;

  virtual void close() = 0;
};

#endif //PROJECT_OPERATOR_HPP

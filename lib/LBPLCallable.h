#ifndef LBPL_CALLABLE_H
#define LBPL_CALLABLE_H

#include "LBPLTypes.h"
#include <vector>

class Interpreter;

class LBPLCallable {
public:
  virtual int arity() = 0;
  virtual Value call(Interpreter*, std::vector<Value>&) = 0;
};

#endif

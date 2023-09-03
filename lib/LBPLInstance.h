#ifndef LBPL_INSTANCE_H
#define LBPL_INSTANCE_H

#include "LBPLClass.h"
#include "runtime_error.h"

#include <map>
#include <memory>

class LBPLInstance {
private:
  LBPLClass *lbplClass;
  std::map<std::string, LBPLType> fields;

public:
  LBPLInstance(LBPLClass *lbplClass) : lbplClass(lbplClass), fields() {}
  LBPLInstance(LBPLInstance *other)
      : lbplClass(other->lbplClass), fields(other->fields) {}

  LBPLType get(const Token *name);
  void set(const Token *name, LBPLType &value);
};

#endif

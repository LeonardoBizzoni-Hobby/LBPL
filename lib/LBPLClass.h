#ifndef LBPL_CLASS_H
#define LBPL_CLASS_H

#include "LBPLCallable.h"
#include "LBPLFunction.h"

#include <map>
#include <memory>

class LBPLClass : public LBPLCallable {
public:
  std::string name;
  std::shared_ptr<LBPLClass> superclass;
  std::map<std::string, LBPLFunc *> methods;

public:
  LBPLClass(const std::string &name, std::shared_ptr<LBPLClass> &superclass,
            std::map<std::string, LBPLFunc *> &methods)
      : name(name), superclass(superclass), methods(methods) {}
  LBPLClass(const std::string &name, std::map<std::string, LBPLFunc *> &methods)
      : name(name), superclass(nullptr), methods(methods) {}

  LBPLFunc *findMethod(const std::string &);

  int arity() override;
  LBPLType call(Interpreter *, std::vector<LBPLType> &) override;
};

#endif

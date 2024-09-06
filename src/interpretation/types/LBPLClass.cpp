#include "LBPLClass.hpp"
#include "../interpreter.hpp"
#include "LBPLInstance.hpp"

Value LBPLClass::call(Interpreter *interpreter, std::vector<Value> &args) {
  auto instance = std::make_shared<LBPLInstance>(this);

  LBPLFunc *init = findMethod("init");
  if (init) {
    init->bind(instance);
    init->call(interpreter, args);
  }

  return instance;
}

LBPLFunc *LBPLClass::findMethod(const std::string &name) {
  if (methods.contains(name)) {
    return methods.find(name)->second;
  } else if (superclass) {
    return superclass->findMethod(name);
  }

  return nullptr;
}

int LBPLClass::arity() {
  auto it = methods.find("init");
  return it == methods.end() ? 0 : it->second->arity();
}

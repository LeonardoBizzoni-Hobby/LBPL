#include "LBPLInstance.hpp"
#include "runtime_error.hpp"

Value LBPLInstance::get(const Token *name) {
  if (fields.contains(name->lexeme)) {
    return fields.find(name->lexeme)->second;
  }

  LBPLFunc *method = lbplClass->findMethod(name->lexeme);
  if (method) {
    method->bind(std::make_shared<LBPLInstance>(this));
    return std::make_shared<LBPLFunc>(*method);
  }

  throw RuntimeError(name, "Undefined field '" + name->lexeme + "'.");
}

void LBPLInstance::set(const Token *name, Value &value) {
  fields.insert_or_assign(name->lexeme, value);
}

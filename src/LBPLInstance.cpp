#include "LBPLInstance.hpp"
#include "runtime_error.hpp"
#include <string>

Value LBPLInstance::get(const Token *name) {
  const char *lexeme = std::get<const char *>(name->lexeme);
  if (fields.contains(lexeme)) {
    return fields.find(lexeme)->second;
  }

  LBPLFunc *method = lbplClass->findMethod(lexeme);
  if (method) {
    method->bind(std::make_shared<LBPLInstance>(this));
    return std::make_shared<LBPLFunc>(*method);
  }

  throw RuntimeError(name, "Undefined field '" + std::string(lexeme) + "'.");
}

void LBPLInstance::set(const Token *name, Value &value) {
  fields.insert_or_assign(std::get<const char *>(name->lexeme), value);
}

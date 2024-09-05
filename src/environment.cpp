#include "environment.hpp"
#include "LBPLTypes.hpp"
#include "runtime_error.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>

void Environment::define(const std::string &name, Value &value) {
  env.insert(std::make_pair(name, value));
}
void Environment::define(const std::string &name, Value &&value) {
  env.insert(std::make_pair(name, value));
}

Value Environment::get(std::shared_ptr<const Token> &name) {
  const char *namestr = std::get<const char *>(name->lexeme);
  auto it = env.find(namestr);

  if (it != env.end()) {
    return it->second;
  }
  if (enclosing) {
    return enclosing->get(name);
  }

  throw RuntimeError(name.get(),
                     "Undefined name '" + std::string(namestr) + "'.");
}

Value Environment::getAt(int depth, std::shared_ptr<const Token> &name) {
  return getAt(depth, std::get<const char *>(name->lexeme));
}

Value Environment::getAt(int depth, const std::string &name) {
  if (depth > 0) {
    return enclosing->getAt(depth - 1, name);
  }

  auto it = env.find(name);
  return it != env.end() ? it->second : nullptr;
}

void Environment::assign(std::shared_ptr<const Token> &name, Value &value) {
  if (auto namestr = std::get<const char *>(name->lexeme);
      env.contains(namestr)) {
    env.insert_or_assign(namestr, value);
  } else if (enclosing) {
    enclosing->assign(name, value);
  } else {
    throw RuntimeError(name.get(),
                       "Undefined variable '" + std::string(namestr) + "'.");
  }
}

void Environment::assign(std::shared_ptr<const Token> &name, Value &&value) {
  assign(name, value);
}

void Environment::assignAt(int depth, std::shared_ptr<const Token> &name,
                           Value &value) {
  if (depth > 0) {
    enclosing->assignAt(depth - 1, name, value);
  }

  env.insert_or_assign(std::get<const char *>(name->lexeme), value);
}

void Environment::printEnv(const std::string &&msg) {
  std::cout << "========" << msg << "=========" << std::endl;
  for (const auto &[key, value] : env) {
    std::cout << "\t" << key << ": ";

    if (std::holds_alternative<int>(value)) {
      std::cout << "int" << std::endl;
    } else if (std::holds_alternative<double>(value)) {
      std::cout << "double" << std::endl;
    } else if (std::holds_alternative<char>(value)) {
      std::cout << "char" << std::endl;
    } else if (std::holds_alternative<std::nullptr_t>(value)) {
      std::cout << "nullptr" << std::endl;
    } else if (std::holds_alternative<std::shared_ptr<LBPLCallable>>(value)) {
      std::cout << "callable" << std::endl;
    } else if (std::holds_alternative<std::shared_ptr<LBPLClass>>(value)) {
      std::cout << "class" << std::endl;
    } else if (std::holds_alternative<std::shared_ptr<LBPLInstance>>(value)) {
      std::cout << "instance" << std::endl;
    } else {
      std::cout << "idk" << std::endl;
    }
  }
  std::cout << "===================================" << std::endl;
}

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "LBPLTypes.h"
#include "token.h"
#include "tree_nodes.h"

#include <map>
#include <memory>
#include <string>
#include <variant>

class Environment {
public:
  std::map<std::string, Value> env;
  std::shared_ptr<Environment> enclosing;

public:
  Environment() : enclosing(nullptr) {}
  Environment(Environment &other)
      : env(other.env),
        enclosing(other.enclosing ? other.enclosing->clone() : nullptr) {}
  Environment(std::shared_ptr<Environment> &enclosing) : enclosing(enclosing) {}

  std::shared_ptr<Environment> clone() {
    return std::make_shared<Environment>(*this);
  }

  void define(const std::string &, Value &);
  void define(const std::string &, Value &&);

  void printEnv(const std::string &&);

  Value get(std::shared_ptr<const Token> &);
  Value getAt(int, std::shared_ptr<const Token> &);
  Value getAt(int, const std::string &);
  void assign(std::shared_ptr<const Token> &, Value &);
  void assign(std::shared_ptr<const Token> &, Value &&);
  void assignAt(int, std::shared_ptr<const Token> &, Value &);
};

#endif

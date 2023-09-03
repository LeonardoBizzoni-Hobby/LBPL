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
  std::map<std::string, LBPLType> env;
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

  void define(const std::string &, LBPLType &);
  void define(const std::string &, LBPLType &&);

  void printEnv(const std::string &&);

  LBPLType get(std::shared_ptr<const Token> &);
  LBPLType getAt(int, std::shared_ptr<const Token> &);
  LBPLType getAt(int, const std::string &);
  void assign(std::shared_ptr<const Token> &, LBPLType &);
  void assign(std::shared_ptr<const Token> &, LBPLType &&);
  void assignAt(int, std::shared_ptr<const Token> &, LBPLType &);
};

#endif

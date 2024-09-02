#ifndef RUNTIME_ERROR_H
#define RUNTIME_ERROR_H

#include "expressions.hpp"
#include "statements.hpp"
#include "token.hpp"

#include <string>

struct RuntimeError {
public:
  int line;
  int column;
  std::string filename;
  std::string msg;

public:
  RuntimeError(const Token *errToken, const std::string &msg)
      : line(errToken->line), column(errToken->column),
        filename(errToken->filename), msg(msg) {}

  RuntimeError(Expr *errToken, const std::string &msg)
      : line(errToken->line), column(errToken->column),
        filename(errToken->file), msg(msg) {}

  RuntimeError(Stmt *errToken, const std::string &msg)
      : line(errToken->line), column(errToken->column),
        filename(errToken->filename), msg(msg) {}

  std::string what();
};

#endif

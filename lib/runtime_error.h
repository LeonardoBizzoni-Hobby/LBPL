#ifndef RUNTIME_ERROR_H
#define RUNTIME_ERROR_H

#include "token.h"
#include "expressions.h"
#include "statements.h"

#include <string>
#include <optional>
#include <iomanip>
#include <sstream>

struct RuntimeError {
public:
  int line;
  int column;
  std::string filename;
  std::string msg;

public:
  RuntimeError(const Token *errToken, const std::string &msg)
      : line(errToken->line), column(errToken->column), filename(errToken->filename), msg(msg) {}

  RuntimeError(Expr *errToken, const std::string &msg)
      : line(errToken->line), column(errToken->column), filename(errToken->file), msg(msg) {}

  RuntimeError(Stmt *errToken, const std::string &msg)
      : line(errToken->line), column(errToken->column), filename(errToken->filename), msg(msg) {}

  std::string what();
};

#endif

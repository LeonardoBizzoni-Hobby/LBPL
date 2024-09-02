#ifndef SYNTAX_ERROR_H
#define SYNTAX_ERROR_H

#include "expressions.hpp"
#include "statements.hpp"
#include "token.hpp"

#include <string>

struct SyntaxError {
public:
  int line;
  int column;
  std::string filename;
  std::string msg;

public:
  SyntaxError(const Token *errToken, const std::string &msg)
      : line(errToken->line), column(errToken->column),
        filename(errToken->filename), msg(msg) {}

  SyntaxError(Expr *errExpr, const std::string &msg)
      : line(errExpr->line), column(errExpr->column), filename(errExpr->file),
        msg(msg) {}

  SyntaxError(Stmt *errStmt, const std::string &msg)
      : line(errStmt->line), column(errStmt->column),
        filename(errStmt->filename), msg(msg) {}

  std::string what();
};

#endif

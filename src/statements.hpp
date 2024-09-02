#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "expressions.hpp"
#include <memory>

struct Stmt {
  std::string filename;
  int line, column;

  Stmt(int line, int column, const std::string &filename)
      : line(line), column(column), filename(filename) {}

  virtual ~Stmt() {}
  virtual void accept(Statement::Visitor *) = 0;
};

struct FnStmt : public Stmt {
  std::shared_ptr<const Token> name;
  std::vector<std::shared_ptr<const Token>> args;
  std::vector<std::unique_ptr<Stmt>> body;

  FnStmt(int line, int column, const std::string &file,
         std::shared_ptr<const Token> &name,
         std::vector<std::shared_ptr<const Token>> &args,
         std::vector<std::unique_ptr<Stmt>> &&body)
      : name(name), args(args), body(std::move(body)),
        Stmt(line, column, file) {}

  void accept(Statement::Visitor *visitor) { visitor->visitFnStmt(this); }
};

struct VarStmt : public Stmt {
  std::shared_ptr<const Token> name;
  std::unique_ptr<Expr> value;

  VarStmt(int line, int column, const std::string &file,
          std::shared_ptr<const Token> &name, std::unique_ptr<Expr> &value)
      : name(name), value(std::move(value)), Stmt(line, column, file) {}

  void accept(Statement::Visitor *visitor) { visitor->visitVarStmt(this); }
};

struct ClassStmt : public Stmt {
  std::shared_ptr<const Token> name;
  std::unique_ptr<VariableExpr> superclass;
  std::vector<std::unique_ptr<Stmt>> body;

  ClassStmt(int line, int column, const std::string &file,
            std::shared_ptr<const Token> &name,
            std::unique_ptr<VariableExpr> &superclass,
            std::vector<std::unique_ptr<Stmt>> &&body)
      : name(name), superclass(std::move(superclass)), body(std::move(body)),
        Stmt(line, column, file) {}

  void accept(Statement::Visitor *visitor) { visitor->visitClassStmt(this); }
};

struct IfStmt : public Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> trueBranch;
  std::unique_ptr<Stmt> falseBranch;

  IfStmt(int line, int column, const std::string &file,
         std::unique_ptr<Expr> &condition, std::unique_ptr<Stmt> &trueBranch,
         std::unique_ptr<Stmt> &falseBranch)
      : condition(std::move(condition)), trueBranch(std::move(trueBranch)),
        falseBranch(std::move(falseBranch)), Stmt(line, column, file) {}

  void accept(Statement::Visitor *visitor) { visitor->visitIfStmt(this); }
};

struct WhileStmt : public Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> body;

  WhileStmt(int line, int column, const std::string &file,
            std::unique_ptr<Expr> &cond, std::unique_ptr<Stmt> &body)
      : condition(std::move(cond)), body(std::move(body)),
        Stmt(line, column, file) {}

  WhileStmt(int line, int column, const std::string &file,
            std::unique_ptr<Expr> &cond, std::unique_ptr<Stmt> &&body)
      : condition(std::move(cond)), body(std::move(body)),
        Stmt(line, column, file) {}

  void accept(Statement::Visitor *visitor) { visitor->visitWhileStmt(this); }
};

struct ForStmt : public Stmt {
  std::unique_ptr<Expr> increment, condition;
  std::unique_ptr<Stmt> initializer, body;

  ForStmt(int line, int column, const std::string &file,
          std::unique_ptr<Stmt> &initializer, std::unique_ptr<Expr> &cond,
          std::unique_ptr<Expr> &increment, std::unique_ptr<Stmt> &body)
      : initializer(std::move(initializer)), condition(std::move(cond)),
        increment(std::move(increment)), body(std::move(body)),
        Stmt(line, column, file) {}

  void accept(Statement::Visitor *visitor) { visitor->visitForStmt(this); }
};

struct ScopedStmt : public Stmt {
  std::vector<std::unique_ptr<Stmt>> body;

  ScopedStmt(int line, int column, const std::string &file,
             std::vector<std::unique_ptr<Stmt>> &&body)
      : body(std::move(body)), Stmt(line, column, file) {}
  void accept(Statement::Visitor *visitor) { visitor->visitScopedStmt(this); }
};

struct ExprStmt : public Stmt {
  std::unique_ptr<Expr> expr;

  ExprStmt(int line, int column, const std::string &file,
           std::unique_ptr<Expr> &&expr)
      : expr(std::move(expr)), Stmt(line, column, file) {}

  void accept(Statement::Visitor *visitor) { visitor->visitExprStmt(this); }
};

struct ReturnStmt : public Stmt {
  std::unique_ptr<Expr> value;

  ReturnStmt(int line, int column, const std::string &file,
             std::unique_ptr<Expr> &value)
      : value(std::move(value)), Stmt(line, column, file) {}

  void accept(Statement::Visitor *visitor) { visitor->visitReturnStmt(this); }
};

#endif

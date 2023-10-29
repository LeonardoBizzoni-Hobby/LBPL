#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "common.h"
#include "token.h"
#include "visitor.h"

#include <cstdint>
#include <memory>
#include <vector>

struct Expr {
  std::string file;
  int line, column;

  Expr(int line, int column, const std::string &filename)
      : line(line), column(column), file(filename) {}

  virtual ~Expr(){};
  virtual LBPLType accept(Expression::Visitor *) { return nullptr; }
};

struct BinaryExpr : public Expr {
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  std::shared_ptr<const Token> op;

  BinaryExpr(int line, int column, const std::string &file,
             std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right,
             std::shared_ptr<const Token> &op)
      : left(std::move(left)), right(std::move(right)), op(op),
        Expr(line, column, file) {}
  BinaryExpr(int line, int column, const std::string &file,
             std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &&right,
             std::shared_ptr<const Token> &op)
      : left(std::move(left)), right(std::move(right)), op(op),
        Expr(line, column, file) {}

  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitBinaryExpr(this);
  }
};

struct BreakExpr : public Expr {
  BreakExpr(int line, int column, const std::string &file)
      : Expr(line, column, file) {}

  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitBreakExpr(this);
  }
};

struct ContinueExpr : public Expr {
  ContinueExpr(int line, int column, const std::string &file)
      : Expr(line, column, file) {}
  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitContinueExpr(this);
  }
};

struct UnaryExpr : public Expr {
  std::unique_ptr<Expr> right;
  std::shared_ptr<const Token> op;

  UnaryExpr(int line, int column, const std::string &file,
            std::unique_ptr<Expr> right, std::shared_ptr<const Token> &op)
      : right(std::move(right)), op(op), Expr(line, column, file) {}
  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitUnaryExpr(this);
  }
};

struct LiteralExpr : public Expr {
  std::shared_ptr<const Token> token;

  LiteralExpr(int line, int column, const std::string &file,
              std::shared_ptr<const Token> &literal)
      : token(literal), Expr(line, column, file) {}
  LiteralExpr(int line, int column, const std::string &file,
              std::shared_ptr<const Token> &&literal)
      : token(literal), Expr(line, column, file) {}
  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitLiteralExpr(this);
  }
};

struct SuperExpr : public Expr {
  std::shared_ptr<const Token> field;

  SuperExpr(int line, int column, const std::string &file,
            std::shared_ptr<const Token> &field)
      : field(field), Expr(line, column, file) {}
  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitSuperExpr(this);
  }
};

struct ThisExpr : public Expr {
public:
  std::shared_ptr<const Token> keyword;

  ThisExpr(int line, int column, const std::string &file, std::shared_ptr<const Token>& keyword)
      : keyword(keyword), Expr(line, column, file) {}

  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitThisExpr(this);
  }
};

struct GroupingExpr : public Expr {
  std::unique_ptr<Expr> expr;

  GroupingExpr(int line, int column, const std::string &file,
               std::unique_ptr<Expr> &expr)
      : expr(std::move(expr)), Expr(line, column, file) {}
  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitGroupExpr(this);
  }
};

struct VariableExpr : public Expr {
  std::shared_ptr<const Token> variable;

  VariableExpr(int line, int column, const std::string &file,
               std::shared_ptr<const Token> &variable)
      : variable(variable), Expr(line, column, file) {}
  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitVarExpr(this);
  }
};

struct AssignExpr : public Expr {
  std::shared_ptr<const Token> variable;
  std::unique_ptr<Expr> value;

  AssignExpr(int line, int column, const std::string &file,
             std::shared_ptr<const Token> &variable,
             std::unique_ptr<Expr> &value)
      : variable(variable), value(std::move(value)), Expr(line, column, file) {}

  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitAssignExpr(this);
  }
};

struct FnCallExpr : public Expr {
  std::unique_ptr<Expr> callee;
  std::vector<std::unique_ptr<Expr>> args;

  FnCallExpr(int line, int column, const std::string &file,
             std::unique_ptr<Expr> &callee,
             std::vector<std::unique_ptr<Expr>> &args)
      : callee(std::move(callee)), args(std::move(args)),
        Expr(line, column, file) {}

  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitCallExpr(this);
  }
};

struct TernaryExpr : public Expr {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Expr> trueBranch;
  std::unique_ptr<Expr> falseBranch;

  TernaryExpr(int line, int column, const std::string &file,
              std::unique_ptr<Expr> &condition,
              std::unique_ptr<Expr> &trueBranch,
              std::unique_ptr<Expr> &&falseBranch)
      : condition(std::move(condition)), trueBranch(std::move(trueBranch)),
        falseBranch(std::move(falseBranch)), Expr(line, column, file) {}

  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitTernaryExpr(this);
  }
};

struct GetFieldExpr : public Expr {
  std::shared_ptr<const Token> field;
  std::unique_ptr<Expr> instance;

  GetFieldExpr(int line, int column, const std::string &file,
               std::unique_ptr<Expr> &instance,
               std::shared_ptr<const Token> &field)
      : instance(std::move(instance)), field(field), Expr(line, column, file) {}

  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitGetFieldExpr(this);
  }
};

struct SetFieldExpr : public Expr {
  std::shared_ptr<const Token> field;
  std::unique_ptr<Expr> value;
  std::unique_ptr<Expr> instance;

  SetFieldExpr(int line, int column, const std::string &file,
               std::unique_ptr<Expr> &instance,
               std::shared_ptr<const Token> &field,
               std::unique_ptr<Expr> &value)
      : instance(std::move(instance)), field(field), value(std::move(value)),
        Expr(line, column, file) {}

  LBPLType accept(Expression::Visitor *visitor) {
    return visitor->visitSetFieldExpr(this);
  }
};

#endif

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "LBPLTypes.h"
#include "builtin_methods.h"
#include "environment.h"
#include "runtime_error.h"
#include "visitor.h"

#include <map>
#include <memory>
#include <vector>

class BreakException {};
class ContinueException {};
class ReturnException {
public:
  Value value;

public:
  ReturnException(Value &&value) : value(value) {}
};

class Interpreter : Statement::Visitor, Expression::Visitor {
private:
  std::shared_ptr<Environment> global, currentEnv;
  std::map<Expr *, int> locals;

private:
  void execute(std::unique_ptr<Stmt> &);

  Value evaluate(std::unique_ptr<Expr> &);
  Value lookupVariable(std::shared_ptr<const Token> &, Expr *);
  bool isTruthy(const Value &);
  bool isTruthy(Value &&);

  Value performBinaryOperation(std::shared_ptr<const Token> &,
                                  const Value &, const Value &);

  void visitFnStmt(FnStmt *) override;
  void visitVarStmt(VarStmt *) override;
  void visitClassStmt(ClassStmt *) override;
  void visitIfStmt(IfStmt *) override;
  void visitWhileStmt(WhileStmt *) override;
  void visitForStmt(ForStmt *) override;
  void visitScopedStmt(ScopedStmt *) override;
  void visitExprStmt(ExprStmt *) override;
  void visitReturnStmt(ReturnStmt *) override;

  Value visitBinaryExpr(BinaryExpr *) override;
  Value visitBreakExpr(BreakExpr *) override;
  Value visitContinueExpr(ContinueExpr *) override;
  Value visitUnaryExpr(UnaryExpr *) override;
  Value visitLiteralExpr(LiteralExpr *) override;
  Value visitGroupExpr(GroupingExpr *) override;
  Value visitSuperExpr(SuperExpr *) override;
  Value visitThisExpr(ThisExpr *) override;
  Value visitCallExpr(FnCallExpr *) override;
  Value visitGetFieldExpr(GetFieldExpr *) override;
  Value visitSetFieldExpr(SetFieldExpr *) override;
  Value visitTernaryExpr(TernaryExpr *) override;
  Value visitVarExpr(VariableExpr *) override;
  Value visitAssignExpr(AssignExpr *) override;

public:
  void executeBlock(std::vector<std::unique_ptr<Stmt>> &,
                    std::shared_ptr<Environment> &);
  void executeBlock(std::vector<std::unique_ptr<Stmt>> &,
                    std::shared_ptr<Environment> &&);
  void interpret(std::vector<std::unique_ptr<Stmt>> &);
  void resolve(Expr *, int);

  Interpreter()
      : global(std::make_shared<Environment>()), currentEnv(global), locals() {
    global->define("println", std::make_shared<LBPLPrintln>());
    global->define("clock", std::make_shared<LBPLClock>());
  }
};

#endif

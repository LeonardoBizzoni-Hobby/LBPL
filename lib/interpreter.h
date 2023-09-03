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
  LBPLType value;

public:
  ReturnException(LBPLType &&value) : value(value) {}
};

class Interpreter : Statement::Visitor, Expression::Visitor {
private:
  std::shared_ptr<Environment> global, currentEnv;
  std::map<Expr *, int> locals;

private:
  void execute(std::unique_ptr<Stmt> &);

  LBPLType evaluate(std::unique_ptr<Expr> &);
  LBPLType lookupVariable(std::shared_ptr<const Token> &, Expr *);
  bool isTruthy(const LBPLType &);
  bool isTruthy(LBPLType &&);

  LBPLType performBinaryOperation(std::shared_ptr<const Token> &,
                                  const LBPLType &, const LBPLType &);

  void visitFnStmt(FnStmt *) override;
  void visitVarStmt(VarStmt *) override;
  void visitClassStmt(ClassStmt *) override;
  void visitIfStmt(IfStmt *) override;
  void visitWhileStmt(WhileStmt *) override;
  void visitForStmt(ForStmt *) override;
  void visitScopedStmt(ScopedStmt *) override;
  void visitExprStmt(ExprStmt *) override;
  void visitReturnStmt(ReturnStmt *) override;

  LBPLType visitBinaryExpr(BinaryExpr *) override;
  LBPLType visitBreakExpr(BreakExpr *) override;
  LBPLType visitContinueExpr(ContinueExpr *) override;
  LBPLType visitUnaryExpr(UnaryExpr *) override;
  LBPLType visitLiteralExpr(LiteralExpr *) override;
  LBPLType visitGroupExpr(GroupingExpr *) override;
  LBPLType visitSuperExpr(SuperExpr *) override;
  LBPLType visitThisExpr(ThisExpr *) override;
  LBPLType visitCallExpr(FnCallExpr *) override;
  LBPLType visitGetFieldExpr(GetFieldExpr *) override;
  LBPLType visitSetFieldExpr(SetFieldExpr *) override;
  LBPLType visitTernaryExpr(TernaryExpr *) override;
  LBPLType visitVarExpr(VariableExpr *) override;
  LBPLType visitAssignExpr(AssignExpr *) override;

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

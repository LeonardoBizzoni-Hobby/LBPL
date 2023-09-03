#ifndef RESOLVER_H
#define RESOLVER_H

#include "environment.h"
#include "interpreter.h"
#include "statements.h"
#include "syntax_error.h"
#include "visitor.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace FunctionType {
enum Type {
  None,
  Function,
  Initializer,
};
}

namespace ClassType {
enum Type {
  None,
  Subclass,
};
}

enum VarState {
  None,
  Init,
  Ready,
};

class Resolver : Statement::Visitor, Expression::Visitor {
private:
  Interpreter *interpreter;
  FunctionType::Type currentFn;
  ClassType::Type currentClass;
  int loops;
  std::vector<std::map<std::string, VarState>> scopes;

public:
  bool hadError;

private:
  void beginScope() { scopes.emplace_back(std::map<std::string, VarState>()); }
  void endScope() { scopes.pop_back(); }

  void declare(const Token *);
  void define(const Token *);

  void resolveLocal(Expr *, const std::string &);
  void resolveLocal(Expr *, const Token *);
  void resolveFunction(FnStmt *, FunctionType::Type);

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
  Resolver(Interpreter *interpreter)
      : interpreter(interpreter), currentFn(FunctionType::None),
        currentClass(ClassType::None), loops(0), scopes() {}

  void resolve(std::vector<std::unique_ptr<Stmt>> &);
};

#endif

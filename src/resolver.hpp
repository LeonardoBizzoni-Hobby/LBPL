#ifndef RESOLVER_H
#define RESOLVER_H

#include "interpreter.hpp"
#include "statements.hpp"
#include "visitor.hpp"

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
  Resolver(Interpreter *interpreter)
      : interpreter(interpreter), currentFn(FunctionType::None),
        currentClass(ClassType::None), loops(0), scopes() {}

  void resolve(std::vector<std::unique_ptr<Stmt>> &);
};

#endif

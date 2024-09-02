#ifndef VISITOR_H
#define VISITOR_H

#include "LBPLTypes.hpp"
#include "tree_nodes.hpp"

namespace Statement {
struct Visitor {
  virtual void visitFnStmt(FnStmt *) = 0;
  virtual void visitVarStmt(VarStmt *) = 0;
  virtual void visitClassStmt(ClassStmt *) = 0;
  virtual void visitIfStmt(IfStmt *) = 0;
  virtual void visitWhileStmt(WhileStmt *) = 0;
  virtual void visitForStmt(ForStmt *) = 0;
  virtual void visitScopedStmt(ScopedStmt *) = 0;
  virtual void visitExprStmt(ExprStmt *) = 0;
  virtual void visitReturnStmt(ReturnStmt *) = 0;
};
} // namespace Statement

namespace Expression {
struct Visitor {
  virtual Value visitBinaryExpr(BinaryExpr *) = 0;
  virtual Value visitBreakExpr(BreakExpr *) = 0;
  virtual Value visitContinueExpr(ContinueExpr *) = 0;
  virtual Value visitUnaryExpr(UnaryExpr *) = 0;
  virtual Value visitLiteralExpr(LiteralExpr *) = 0;
  virtual Value visitGroupExpr(GroupingExpr *) = 0;
  virtual Value visitSuperExpr(SuperExpr *) = 0;
  virtual Value visitThisExpr(ThisExpr *) = 0;
  virtual Value visitCallExpr(FnCallExpr *) = 0;
  virtual Value visitGetFieldExpr(GetFieldExpr *) = 0;
  virtual Value visitSetFieldExpr(SetFieldExpr *) = 0;
  virtual Value visitTernaryExpr(TernaryExpr *) = 0;
  virtual Value visitVarExpr(VariableExpr *) = 0;
  virtual Value visitAssignExpr(AssignExpr *) = 0;
};
} // namespace Expression

#endif

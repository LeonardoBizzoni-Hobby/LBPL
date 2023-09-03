#ifndef VISITOR_H
#define VISITOR_H

#include "LBPLTypes.h"
#include "tree_nodes.h"

#include <iostream>

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
  virtual LBPLType visitBinaryExpr(BinaryExpr *) = 0;
  virtual LBPLType visitBreakExpr(BreakExpr *) = 0;
  virtual LBPLType visitContinueExpr(ContinueExpr *) = 0;
  virtual LBPLType visitUnaryExpr(UnaryExpr *) = 0;
  virtual LBPLType visitLiteralExpr(LiteralExpr *) = 0;
  virtual LBPLType visitGroupExpr(GroupingExpr *) = 0;
  virtual LBPLType visitSuperExpr(SuperExpr *) = 0;
  virtual LBPLType visitThisExpr(ThisExpr *) = 0;
  virtual LBPLType visitCallExpr(FnCallExpr *) = 0;
  virtual LBPLType visitGetFieldExpr(GetFieldExpr *) = 0;
  virtual LBPLType visitSetFieldExpr(SetFieldExpr *) = 0;
  virtual LBPLType visitTernaryExpr(TernaryExpr *) = 0;
  virtual LBPLType visitVarExpr(VariableExpr *) = 0;
  virtual LBPLType visitAssignExpr(AssignExpr *) = 0;
};
} // namespace Expression

#endif

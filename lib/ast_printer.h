#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "tree_nodes.h"
#include "visitor.h"

class AST_Printer : public Statement::Visitor, public Expression::Visitor {
  void printLocation(Stmt *);

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
};

#endif

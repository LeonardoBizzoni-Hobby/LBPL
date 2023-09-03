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
};

#endif

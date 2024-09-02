#include "ast_printer.hpp"
#include "statements.hpp"

#include <iostream>

void AST_Printer::printLocation(Stmt *stmt) {
  std::cout << "Location: " << stmt->filename << " [Line: " << stmt->line
            << ", Column: " << stmt->column << "]\n";
}

void AST_Printer::visitFnStmt(FnStmt *stmt) {
  printLocation(stmt);
  std::cout << "(";
  std::cout << "fn `" + stmt->name->lexeme + "` args[";

  for (auto &&arg : stmt->args) {
    std::cout << "(" << arg->lexeme << ")";
  }

  std::cout << "] ` body {\n";
  for (auto &&bodyStmt : stmt->body) {
    bodyStmt->accept(this);
  }

  std::cout << "})\n";
}

void AST_Printer::visitVarStmt(VarStmt *var) {
  printLocation(var);
  std::cout << "(var " << var->name->lexeme << " value `";

  if (var->value) {
    var->value->accept(this);
  } else {
    std::cout << "nil";
  }
  std::cout << "`)\n";
}

void AST_Printer::visitClassStmt(ClassStmt *stmt) {
  printLocation(stmt);
  std::cout << "(class `" << stmt->name->lexeme << "` implements(";
  if (stmt->superclass) {
    stmt->superclass->accept(this);
  }
  std::cout << ") body {\n";
  for (auto &&fn : stmt->body) {
    fn->accept(this);
  }
  std::cout << "})\n";
}

void AST_Printer::visitIfStmt(IfStmt *fn) {
  printLocation(fn);
  std::cout << "(condition `";
  fn->condition->accept(this);
  std::cout << "` if true ";
  fn->trueBranch->accept(this);
  std::cout << " if false ";
  if (fn->falseBranch) {
    fn->falseBranch->accept(this);
  }
  std::cout << ")\n";
}

void AST_Printer::visitWhileStmt(WhileStmt *stmt) {
  printLocation(stmt);
  std::cout << "(while ";
  stmt->condition->accept(this);
  std::cout << " do ";
  stmt->body->accept(this);
  std::cout << ")\n";
}

void AST_Printer::visitForStmt(ForStmt *stmt) {
  printLocation(stmt);
  std::cout << "(for ";
  if (stmt->initializer) {
    stmt->initializer->accept(this);
  }
  std::cout << " while ";
  if (stmt->condition) {
    stmt->condition->accept(this);
  } else {
    std::cout << "yes";
  }
  std::cout << " do ";
  stmt->body->accept(this);
  std::cout << " then ";
  if (stmt->increment) {
    stmt->increment->accept(this);
  }
  std::cout << ")\n";
}

void AST_Printer::visitScopedStmt(ScopedStmt *stmt) {
  printLocation(stmt);
  std::cout << "{";
  for (auto &&stmt : stmt->body) {
    stmt->accept(this);
  }
  std::cout << "}";
}

void AST_Printer::visitExprStmt(ExprStmt *stmt) {
  printLocation(stmt);
  stmt->expr->accept(this);
  std::cout << "\n";
}

void AST_Printer::visitReturnStmt(ReturnStmt *stmt) {
  printLocation(stmt);
  std::cout << "(return ";
  if (stmt->value) {
    stmt->value->accept(this);
  } else {
    std::cout << "void";
  }
  std::cout << ")\n";
}

Value AST_Printer::visitBinaryExpr(BinaryExpr *expr) {
  std::cout << "(" << expr->op->lexeme << " ";
  expr->left->accept(this);
  std::cout << " ";
  expr->right->accept(this);
  std::cout << ")";
  return nullptr;
}

Value AST_Printer::visitBreakExpr(BreakExpr *expr) {
  std::cout << "(breaking)";
  return nullptr;
}

Value AST_Printer::visitContinueExpr(ContinueExpr *expr) {
  std::cout << "(continuing)\n";
  return nullptr;
}

Value AST_Printer::visitUnaryExpr(UnaryExpr *expr) {
  std::cout << "(" << expr->op->lexeme << " ";
  expr->right->accept(this);
  std::cout << ")";
  return nullptr;
}

Value AST_Printer::visitLiteralExpr(LiteralExpr *expr) {
  std::cout << expr->token->lexeme;
  return nullptr;
}

Value AST_Printer::visitSuperExpr(SuperExpr *expr) {
  std::cout << "(super)";
  return nullptr;
}

Value AST_Printer::visitThisExpr(ThisExpr *expr) {
  std::cout << "(this)";
  return nullptr;
}

Value AST_Printer::visitAssignExpr(AssignExpr *expr) {
  std::cout << "(assign ";
  expr->value->accept(this);
  std::cout << " to " << expr->variable->lexeme << ")";
  return nullptr;
}

Value AST_Printer::visitGroupExpr(GroupingExpr *expr) {
  expr->expr->accept(this);
  return nullptr;
}

Value AST_Printer::visitCallExpr(FnCallExpr *expr) {
  std::cout << "(calling `";
  expr->callee->accept(this);
  std::cout << "` with parameters [";

  for (auto &&arg : expr->args) {
    arg->accept(this);
  }

  std::cout << "])";
  return nullptr;
}

Value AST_Printer::visitGetFieldExpr(GetFieldExpr *expr) {
  std::cout << "get field `" << expr->field->lexeme << "` from ";
  expr->instance->accept(this);
  return nullptr;
}

Value AST_Printer::visitSetFieldExpr(SetFieldExpr *expr) {
  std::cout << "setting field `" << expr->field->lexeme << "` from ";
  expr->instance->accept(this);
  std::cout << " to ";
  expr->value->accept(this);
  return nullptr;
}

Value AST_Printer::visitTernaryExpr(TernaryExpr *expr) {
  std::cout << "condition `";
  expr->condition->accept(this);
  std::cout << "` if true ";
  expr->trueBranch->accept(this);
  std::cout << " if false ";
  expr->falseBranch->accept(this);
  return nullptr;
}

Value AST_Printer::visitVarExpr(VariableExpr *expr) {
  std::cout << expr->variable->lexeme;
  return nullptr;
}

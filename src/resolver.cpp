#include "resolver.h"

void Resolver::resolve(std::vector<std::unique_ptr<Stmt>> &stmts) {
  for (auto &&stmt : stmts) {
    try {
      stmt->accept(this);
    } catch (SyntaxError &e) {
      std::cout << e.what();
      hadError = true;
    }
  }
}

void Resolver::declare(const Token *name) {
  if (scopes.empty()) {
    return;
  }

  std::map<std::string, VarState> &scope = scopes.back();
  if (scope.contains(name->lexeme)) {
    throw SyntaxError(name, "Variable with this name already exists.");
  }

  scope.insert(std::make_pair(name->lexeme, VarState::Init));
}

void Resolver::define(const Token *name) {
  if (scopes.empty()) {
    return;
  }

  scopes.back().insert_or_assign(name->lexeme, VarState::Ready);
}

void Resolver::resolveLocal(Expr *expr, const Token *name) {
  for (int i = scopes.size() - 1; i >= 0; i--) {
    if (scopes[i].contains(name->lexeme)) {
      interpreter->resolve(expr, scopes.size() - 1 - i);
      return;
    }
  }
}

void Resolver::resolveLocal(Expr *expr, const std::string &name) {
  for (int i = scopes.size() - 1; i >= 0; i--) {
    if (scopes[i].contains(name)) {
      interpreter->resolve(expr, scopes.size() - 1 - i);
      return;
    }
  }
}

void Resolver::resolveFunction(FnStmt *fn, FunctionType::Type type) {
  FunctionType::Type enclosingFn = currentFn;
  currentFn = type;

  beginScope();
  for (auto &&arg : fn->args) {
    declare(arg.get());
    define(arg.get());
  }

  resolve(fn->body);
  endScope();
  currentFn = enclosingFn;
}

void Resolver::visitFnStmt(FnStmt *fn) {
  declare(fn->name.get());
  define(fn->name.get());
  resolveFunction(fn, FunctionType::Function);
}

void Resolver::visitVarStmt(VarStmt *var) {
  declare(var->name.get());
  if (var->value) {
    var->value->accept(this);
  }

  define(var->name.get());
}

void Resolver::visitClassStmt(ClassStmt *clas) {
  declare(clas->name.get());
  define(clas->name.get());

  if (clas->superclass &&
      clas->superclass->variable->lexeme == clas->name->lexeme) {
    throw SyntaxError(clas, "A class can't inherit from itself.");
  }

  if (clas->superclass) {
    currentClass = ClassType::Subclass;
    clas->superclass->accept(this);

    beginScope();
    scopes.back().insert(std::make_pair("super", VarState::Ready));
  } else {
    currentClass = ClassType::None;
  }

  beginScope();
  scopes.back().insert(std::make_pair("this", VarState::Ready));

  for (auto &&stmt : clas->body) {
    auto method = dynamic_cast<FnStmt *>(stmt.get());
    if (method && method->name->lexeme == "init") {
      resolveFunction(method, FunctionType::Initializer);
    } else {
      resolveFunction(method, FunctionType::Function);
    }
  }

  endScope();

  if (clas->superclass) {
    endScope();
  }
}

void Resolver::visitIfStmt(IfStmt *stmt) {
  stmt->condition->accept(this);
  stmt->trueBranch->accept(this);
  if (stmt->falseBranch) {
    stmt->falseBranch->accept(this);
  }
}

void Resolver::visitWhileStmt(WhileStmt *loop) {
  loop->condition->accept(this);
  loops++;
  loop->body->accept(this);
  loops--;
}

void Resolver::visitForStmt(ForStmt *loop) {
  beginScope();
  loop->initializer->accept(this);
  loop->condition->accept(this);
  loop->increment->accept(this);
  loops++;
  loop->body->accept(this);
  loops--;
  endScope();
}

void Resolver::visitScopedStmt(ScopedStmt *block) {
  beginScope();
  resolve(block->body);
  endScope();
}

void Resolver::visitExprStmt(ExprStmt *stmt) { stmt->expr->accept(this); }

void Resolver::visitReturnStmt(ReturnStmt *ret) {
  if (currentFn == FunctionType::None) {
    throw SyntaxError(ret, "Can't return from top-level code");
  } else if (ret->value) {
    if (currentFn == FunctionType::Initializer) {
      throw SyntaxError(ret, "Can't return a value from a class initializer.");
    }

    ret->value->accept(this);
  }
}

Value Resolver::visitBinaryExpr(BinaryExpr *expr) {
  expr->left->accept(this);
  expr->right->accept(this);
  return nullptr;
}

Value Resolver::visitBreakExpr(BreakExpr *expr) {
  if (loops <= 0) {
    throw SyntaxError(expr, "Can't break from outside of a loop.");
  }
  return nullptr;
}

Value Resolver::visitContinueExpr(ContinueExpr *expr) {
  if (loops <= 0) {
    throw SyntaxError(expr, "Can't break from outside of a loop.");
  }
  return nullptr;
}

Value Resolver::visitUnaryExpr(UnaryExpr *expr) {
  expr->right->accept(this);
  return nullptr;
}

Value Resolver::visitLiteralExpr(LiteralExpr *) { return nullptr; }

Value Resolver::visitGroupExpr(GroupingExpr *expr) {
  expr->expr->accept(this);
  return nullptr;
}

Value Resolver::visitSuperExpr(SuperExpr *expr) {
  if (currentClass == ClassType::None) {
    throw SyntaxError(expr, "Can't access 'super' from outside of class body.");
  } else if (currentClass != ClassType::Subclass) {
    throw SyntaxError(expr,
                      "Can't access 'super' in a class without superclass.");
  }

  resolveLocal(expr, "super");
  return nullptr;
}

Value Resolver::visitThisExpr(ThisExpr *expr) {
  resolveLocal(expr, "this");
  return nullptr;
}

Value Resolver::visitCallExpr(FnCallExpr *expr) {
  expr->callee->accept(this);

  for (auto &&arg : expr->args) {
    arg->accept(this);
  }
  return nullptr;
}

Value Resolver::visitGetFieldExpr(GetFieldExpr *expr) {
  expr->instance->accept(this);
  return nullptr;
}

Value Resolver::visitSetFieldExpr(SetFieldExpr *expr) {
  expr->value->accept(this);
  expr->instance->accept(this);
  return nullptr;
}

Value Resolver::visitTernaryExpr(TernaryExpr *expr) {
  expr->condition->accept(this);
  expr->trueBranch->accept(this);
  expr->falseBranch->accept(this);
  return nullptr;
}

Value Resolver::visitVarExpr(VariableExpr *expr) {
  if (!scopes.empty() && scopes.back().contains(expr->variable->lexeme) &&
      scopes.back().find(expr->variable->lexeme)->second != VarState::Ready) {
    throw SyntaxError(expr, "You are trying to read the value of a variable "
                            "that hasn't been defined yet.");
  }

  resolveLocal(expr, expr->variable.get());
  return nullptr;
}

Value Resolver::visitAssignExpr(AssignExpr *expr) {
  expr->value->accept(this);
  resolveLocal(expr, expr->variable.get());
  return nullptr;
}

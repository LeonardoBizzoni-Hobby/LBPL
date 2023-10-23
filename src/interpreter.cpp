#include "interpreter.h"
#include "LBPLClass.h"
#include "LBPLFunction.h"
#include "LBPLInstance.h"
#include "LBPLTypes.h"
#include "runtime_error.h"

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <variant>

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>> &stmts) {
  try {
    for (auto &&stmt : stmts) {
      stmt->accept(this);
    }
  } catch (RuntimeError &e) {
    std::cout << e.what();
  }
}

void Interpreter::resolve(Expr *expr, int depth) {
  locals.insert_or_assign(expr, depth);
}

void Interpreter::visitFnStmt(FnStmt *stmt) {
  currentEnv->define(stmt->name->lexeme,
                     std::make_shared<LBPLFunc>(stmt, currentEnv, false));
}

void Interpreter::visitVarStmt(VarStmt *stmt) {
  LBPLType value;
  if (stmt->value) {
    value = stmt->value->accept(this);
  }

  currentEnv->define(stmt->name->lexeme, value);
}

void Interpreter::visitClassStmt(ClassStmt *stmt) {
  LBPLType superclass;
  currentEnv->define(stmt->name->lexeme, nullptr);

  if (stmt->superclass) {
    superclass = stmt->superclass->accept(this);
    if (!std::holds_alternative<std::shared_ptr<LBPLClass>>(superclass)) {
      throw RuntimeError(stmt->superclass.get(),
                         "Superclass must be another class.");
    }

    currentEnv = std::make_shared<Environment>(currentEnv);
    currentEnv->define("super",
                       std::get<std::shared_ptr<LBPLClass>>(superclass));
  }

  std::map<std::string, LBPLFunc *> methods;
  for (auto &&methodStmt : stmt->body) {
    auto method = dynamic_cast<FnStmt *>(methodStmt.get());
    if (!method) {
      throw RuntimeError(methodStmt.get(), "Excepted class method.");
    }

    auto fn = new LBPLFunc(method, currentEnv, method->name->lexeme == "init");
    methods.insert(std::make_pair(method->name->lexeme, fn));
  }

  if (stmt->superclass) {
    currentEnv = currentEnv->enclosing;
    currentEnv->assign(stmt->name,
                       std::make_shared<LBPLClass>(
                           stmt->name->lexeme,
                           std::get<std::shared_ptr<LBPLClass>>(superclass),
                           methods));
  } else {
    currentEnv->assign(
        stmt->name, std::make_shared<LBPLClass>(stmt->name->lexeme, methods));
  }
}

void Interpreter::visitIfStmt(IfStmt *stmt) {
  if (isTruthy(stmt->condition->accept(this))) {
    stmt->trueBranch->accept(this);
  } else if (stmt->falseBranch) {
    stmt->falseBranch->accept(this);
  }
}

void Interpreter::visitWhileStmt(WhileStmt *stmt) {
  try {
    while (isTruthy(stmt->condition->accept(this))) {
      try {
        stmt->body->accept(this);
      } catch (ContinueException &e) {
      }
    }
  } catch (BreakException &e) {
  }
}

void Interpreter::visitForStmt(ForStmt *stmt) {
  auto env = currentEnv;
  currentEnv = std::make_shared<Environment>(currentEnv);

  stmt->initializer->accept(this);

  try {
    while (isTruthy(stmt->condition->accept(this))) {
      try {
        stmt->body->accept(this);
      } catch (ContinueException &e) {
      }

      stmt->increment->accept(this);
    }
  } catch (BreakException &e) {
  }

  currentEnv = env;
}

void Interpreter::visitScopedStmt(ScopedStmt *stmt) {
  executeBlock(stmt->body, std::make_shared<Environment>(currentEnv));
}

void Interpreter::visitExprStmt(ExprStmt *stmt) { stmt->expr->accept(this); }
void Interpreter::visitReturnStmt(ReturnStmt *stmt) {
  throw ReturnException(stmt->value->accept(this));
}

LBPLType Interpreter::visitBinaryExpr(BinaryExpr *expr) {
  LBPLType left = expr->left->accept(this);
  LBPLType right = expr->right->accept(this);

  return performBinaryOperation(expr->op, left, right);
}

LBPLType Interpreter::visitBreakExpr(BreakExpr *) { throw BreakException(); }
LBPLType Interpreter::visitContinueExpr(ContinueExpr *) {
  throw ContinueException();
}

LBPLType Interpreter::visitUnaryExpr(UnaryExpr *expr) {
  LBPLType right = expr->right->accept(this);

  if (expr->op->type == TokenType::Minus) {
    if (std::holds_alternative<int>(right)) {
      return -std::get<int>(right);
    } else if (std::holds_alternative<double>(right)) {
      return -std::get<double>(right);
    }
  } else if (expr->op->type == TokenType::Bang) {
    return !isTruthy(right);
  }

  return nullptr;
}

LBPLType Interpreter::visitLiteralExpr(LiteralExpr *expr) {
  if (expr->token->type == TokenType::True) {
    return true;
  } else if (expr->token->type == TokenType::False) {
    return false;
  } else if (expr->token->type == TokenType::Nil) {
    return nullptr;
  } else if (expr->token->type == TokenType::Char) {
    return expr->token->lexeme[0];
  } else if (expr->token->type == TokenType::String) {
    return expr->token->lexeme;
  } else if (expr->token->type == TokenType::Number) {
    if (expr->token->lexeme.find('.') == std::string::npos) {
      return std::stoi(expr->token->lexeme);
    }
    return std::stod(expr->token->lexeme);
  } else if (expr->token->type == TokenType::Identifier) {
    return lookupVariable(expr->token, expr);
  }

  return nullptr;
}

LBPLType Interpreter::visitGroupExpr(GroupingExpr *expr) {
  return expr->expr->accept(this);
}

LBPLType Interpreter::visitSuperExpr(SuperExpr *) { return nullptr; }
LBPLType Interpreter::visitThisExpr(ThisExpr *) { return nullptr; }

LBPLType Interpreter::visitCallExpr(FnCallExpr *expr) {
  LBPLType callee = expr->callee->accept(this);

  std::vector<LBPLType> args;
  args.reserve(expr->args.size());

  for (auto &&arg : expr->args) {
    args.emplace_back(arg->accept(this));
  }

  if (std::holds_alternative<std::shared_ptr<LBPLCallable>>(callee)) {
    auto function = std::get<std::shared_ptr<LBPLCallable>>(callee);
    if (function->arity() != args.size()) {
      throw RuntimeError(expr->callee.get(), "Wrong number of arguments.");
    }

    return function->call(this, args);
  } else if (std::holds_alternative<std::shared_ptr<LBPLClass>>(callee)) {
    auto clas = std::get<std::shared_ptr<LBPLClass>>(callee);
    if (clas->arity() != args.size()) {
      throw RuntimeError(expr->callee.get(), "Wrong number of arguments.");
    }

    return clas->call(this, args);
  }

  throw RuntimeError(expr->callee.get(),
                     "Can only call a function or class initializer.");
}

LBPLType Interpreter::visitGetFieldExpr(GetFieldExpr *expr) {
  LBPLType instance = expr->instance->accept(this);
  if (std::holds_alternative<std::shared_ptr<LBPLInstance>>(instance)) {
    return std::get<std::shared_ptr<LBPLInstance>>(instance)->get(
        expr->field.get());
  }

  throw RuntimeError(expr->instance.get(),
                     "Only instances of classes can have properties");
}

LBPLType Interpreter::visitSetFieldExpr(SetFieldExpr *expr) {
  LBPLType instance = expr->instance->accept(this);

  if (std::holds_alternative<std::shared_ptr<LBPLInstance>>(instance)) {
    LBPLType value = expr->value->accept(this);
    std::get<std::shared_ptr<LBPLInstance>>(instance)->set(expr->field.get(),
                                                           value);
  } else {
    throw RuntimeError(expr->instance.get(),
                       "Only instances of classes can have properties");
  }

  return nullptr;
}

LBPLType Interpreter::visitTernaryExpr(TernaryExpr *expr) {
  if (isTruthy(expr->condition->accept(this))) {
    return expr->trueBranch->accept(this);
  }

  return expr->falseBranch->accept(this);
}

LBPLType Interpreter::visitVarExpr(VariableExpr *expr) {
  return lookupVariable(expr->variable, expr);
}
LBPLType Interpreter::visitAssignExpr(AssignExpr *expr) {
  LBPLType value = expr->value->accept(this);

  auto it = locals.find(expr);
  if (it == locals.end()) {
    global->assign(expr->variable, value);
  } else {
    currentEnv->assignAt(it->second, expr->variable, value);
  }

  return value;
}

void Interpreter::execute(std::unique_ptr<Stmt> &stmt) { stmt->accept(this); }

LBPLType Interpreter::evaluate(std::unique_ptr<Expr> &expr) {
  return expr->accept(this);
}

LBPLType Interpreter::performBinaryOperation(std::shared_ptr<const Token> &op,
                                             const LBPLType &left,
                                             const LBPLType &right) {
  auto performIntOp = [](int l, int r,
                         std::shared_ptr<const Token> &op) -> LBPLType {
    switch (op->type) {
    case TokenType::Plus:
      return l + r;
    case TokenType::Minus:
      return l - r;
    case TokenType::Star:
      return l * r;
    case TokenType::Slash:
      if (r == 0) {
        throw RuntimeError(op.get(), "Division by zero.");
      }
      return l / r;
    case TokenType::Less:
      return l < r;
    case TokenType::LessEqual:
      return l <= r;
    case TokenType::Greater:
      return l > r;
    case TokenType::GreaterEqual:
      return l >= r;
    case TokenType::EqualEqual:
      return l == r;
    case TokenType::BangEqual:
      return l != r;
    case TokenType::ModOp:
      if (r == 0) {
        throw RuntimeError(op.get(), "Modulo by zero.");
      }
      return l % r;
    default:
      throw RuntimeError(op.get(), "Unsupported binary operation.");
    }
  };

  auto performDoubleOp = [](double l, double r,
                            std::shared_ptr<const Token> &op) -> LBPLType {
    switch (op->type) {
    case TokenType::Plus:
      return l + r;
    case TokenType::Minus:
      return l - r;
    case TokenType::Star:
      return l * r;
    case TokenType::Slash:
      if (r == 0) {
        throw RuntimeError(op.get(), "Division by zero.");
      }
      return l / r;
    case TokenType::Less:
      return l < r;
    case TokenType::LessEqual:
      return l <= r;
    case TokenType::Greater:
      return l > r;
    case TokenType::GreaterEqual:
      return l >= r;
    case TokenType::EqualEqual:
      return l == r;
    case TokenType::BangEqual:
      return l != r;
    default:
      throw RuntimeError(op.get(), "Unsupported binary operation.");
    }
  };

  auto performStringOp = [](const std::string &l, const std::string &r,
                            std::shared_ptr<const Token> &op) -> LBPLType {
    if (op->type == TokenType::Plus) {
      return l + r;
    } else {
      throw RuntimeError(op.get(), "Unsupported binary operation.");
    }
  };

  return std::visit(
      [&](const auto &l, const auto &r) -> LBPLType {
        using L = std::decay_t<decltype(l)>;
        using R = std::decay_t<decltype(r)>;

        if constexpr (std::is_same_v<L, int> && std::is_same_v<R, int>) {
          return performIntOp(l, r, op);
        } else if constexpr (std::is_same_v<L, double> &&
                             std::is_same_v<R, double>) {
          return performDoubleOp(l, r, op);
        } else if constexpr ((std::is_same_v<L, std::string> &&
                              std::is_same_v<R, int>) ||
                             (std::is_same_v<L, int> &&
                              std::is_same_v<R, std::string>)) {
          // Handle int-to-string or string-to-int concatenation
          std::string leftStr = std::holds_alternative<int>(left)
                                    ? std::to_string(std::get<int>(left))
                                    : std::get<std::string>(left);
          std::string rightStr = std::holds_alternative<int>(right)
                                     ? std::to_string(std::get<int>(right))
                                     : std::get<std::string>(right);
          if (op->type == TokenType::Plus) {
            return leftStr + rightStr;
          } else {
            throw RuntimeError(op.get(), "Unsupported binary operation.");
          }
        } else if constexpr ((std::is_same_v<L, std::string> &&
                              std::is_same_v<R, double>) ||
                             (std::is_same_v<L, double> &&
                              std::is_same_v<R, std::string>)) {
          // Handle double-to-string or string-to-double concatenation
          std::string leftStr = std::holds_alternative<double>(left)
                                    ? std::to_string(std::get<double>(left))
                                    : std::get<std::string>(left);
          std::string rightStr = std::holds_alternative<double>(right)
                                     ? std::to_string(std::get<double>(right))
                                     : std::get<std::string>(right);
          if (op->type == TokenType::Plus) {
            return leftStr + rightStr;
          } else {
            throw RuntimeError(op.get(), "Unsupported binary operation.");
          }
        } else if constexpr (std::is_same_v<L, std::string> &&
                             std::is_same_v<R, std::string>) {
          return performStringOp(l, r, op);
        } else {
          throw RuntimeError(op.get(), "Unsupported binary operation.");
        }
      },
      left, right);
}

void Interpreter::executeBlock(std::vector<std::unique_ptr<Stmt>> &body,
                               std::shared_ptr<Environment> &&env) {
  executeBlock(body, env);
}

void Interpreter::executeBlock(std::vector<std::unique_ptr<Stmt>> &body,
                               std::shared_ptr<Environment> &env) {
  auto prev = currentEnv;

  try {
    currentEnv = env;
    for (auto &&stmt : body) {
      stmt->accept(this);
    }
  } catch (ReturnException &e) {
    currentEnv = prev;
    throw e;
  }

  currentEnv = prev;
}

bool Interpreter::isTruthy(const LBPLType &value) {
  if (std::holds_alternative<std::nullptr_t>(value)) {
    return std::get<std::nullptr_t>(value) != nullptr;
  } else if (std::holds_alternative<bool>(value)) {
    return std::get<bool>(value);
  } else if (std::holds_alternative<int>(value)) {
    return std::get<int>(value) == 0;
  } else if (std::holds_alternative<double>(value)) {
    return std::get<double>(value) == 0;
  }

  return false;
}

bool Interpreter::isTruthy(LBPLType &&value) { return isTruthy(value); }

LBPLType Interpreter::lookupVariable(std::shared_ptr<const Token> &name,
                                     Expr *expr) {
  auto it = locals.find(expr);

  if (it == locals.end()) {
    return global->get(name);
  }

  return currentEnv->getAt(it->second, name);
}

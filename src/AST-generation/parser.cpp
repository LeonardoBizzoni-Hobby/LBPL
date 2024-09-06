#include "parser.hpp"
#include "syntax_error.hpp"

#include <cstdint>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <variant>

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
  std::vector<std::unique_ptr<Stmt>> stmts;

  while (!isAtEnd()) {
    try {
      if (match(TokenType::Import)) {
        for (auto &&stmt : importStmt()) {
          stmts.emplace_back(std::move(stmt));
        }
      } else {
        stmts.emplace_back(declaration());
      }
    } catch (SyntaxError &e) {
      hadError = true;
      std::cout << e.what();
      synchronize();
    }
  }

  return stmts;
}

std::unique_ptr<Stmt> Parser::declaration() {
  if (match(TokenType::Let)) {
    return varDecl();
  } else if (match(TokenType::Fn)) {
    return functionDecl("function");
  } else if (match(TokenType::Class)) {
    return classDecl();
  }

  return statement();
}

std::vector<std::unique_ptr<Stmt>> Parser::importStmt() {
  std::shared_ptr<const Token> path =
      consume("Expected path to file to import but instead got: '" +
                  type2str(current->type) + "'.",
              TokenType::String);

  const char *filepath = std::get<const char *>(path->lexeme);
  if (importedFiles.contains(filepath)) {
    throw SyntaxError(previous.get(),
                      "Recursive file import: '" + std::string(filepath) +
                          "' has already been imported or is the main file.");
  }

  importedFiles.insert(filepath);
  std::ifstream sourceFile(filepath);

  CONSUME_SEMICOLON("import");

  return (new Parser(sourceFile, filepath, importedFiles))->parse();
}

std::unique_ptr<VarStmt> Parser::varDecl() {
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  std::shared_ptr<const Token> name =
      consume("Expected variable name after 'let' keyword but instead got '" +
                  type2str(current->type) + "'.",
              TokenType::Identifier);

  std::unique_ptr<Expr> value;
  if (match(TokenType::Equal)) {
    value = expression();
  }

  consume("Expected ';' at the end of a statement but instead got: '" +
              type2str(current->type) + "'.",
          TokenType::Semicolon);
  return std::make_unique<VarStmt>(line, col, filename, name, value);
}

std::unique_ptr<FnStmt> Parser::functionDecl(const std::string &kind) {
  std::vector<std::shared_ptr<const Token>> args;
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  std::shared_ptr<const Token> name =
      consume("Expected " + kind + " name.", TokenType::Identifier);

  consume("Expected argument list after trait function name but instead got '" +
              type2str(current->type) + "'.",
          TokenType::LeftParen);

  if (!check(TokenType::RightParen)) {
    do {
      std::shared_ptr<const Token> arg = consume(
          "Expected name of trait function parameter but instead got '" +
              type2str(current->type) + "'.",
          TokenType::Identifier);
      args.push_back(arg);
    } while (match(TokenType::Comma));
  }

  consume("Expected ')' but instead got '" + type2str(current->type) + "'.",
          TokenType::RightParen);
  consume("Expected '{' after function signature but instead got '" +
              type2str(current->type) + "'.",
          TokenType::LeftBrace);

  return std::make_unique<FnStmt>(line, col, filename, name, args,
                                  stmtSequence());
}

std::unique_ptr<ClassStmt> Parser::classDecl() {
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  std::shared_ptr<const Token> name = consume(
      "Expected class name but instead got '" + type2str(current->type) + "'.",
      TokenType::Identifier);

  std::unique_ptr<VariableExpr> superclass;
  if (match(TokenType::Colon)) {
    std::shared_ptr<const Token> supername =
        consume("Expected superclass name.", TokenType::Identifier);
    superclass = std::make_unique<VariableExpr>(line, col, filename, supername);
  }

  auto clas = std::make_unique<ClassStmt>(line, col, filename, name, superclass,
                                          std::vector<std::unique_ptr<Stmt>>());
  if (match(TokenType::Semicolon)) {
    return clas;
  } else if (match(TokenType::LeftBrace)) {
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
      clas->body.emplace_back(functionDecl("method"));
    }
    consume("Expected closing '}'.", TokenType::RightBrace);

    return clas;
  } else {
    throw SyntaxError(current.get(),
                      "Expected either a semicolon for an inline class "
                      "definition or a sequence of methods but instead got '" +
                          type2str(current->type) + "'.");
  }
}

std::unique_ptr<Stmt> Parser::statement() {
  if (match(TokenType::LeftBrace)) {
    return scopedStmt();
  } else if (match(TokenType::If)) {
    return ifStmt();
  } else if (match(TokenType::While)) {
    return whileStmt();
  } else if (match(TokenType::Loop)) {
    return loopStmt();
  } else if (match(TokenType::For)) {
    return forStmt();
  } else if (match(TokenType::Return)) {
    return returnStmt();
  }

  return expressionStmt();
}

std::unique_ptr<Stmt> Parser::scopedStmt() {
  return std::make_unique<ScopedStmt>(current->line, current->column,
                                      current->filename, stmtSequence());
}

std::unique_ptr<Stmt> Parser::ifStmt() {
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  std::unique_ptr<Expr> cond = expression();
  std::unique_ptr<Stmt> trueBranch = statement();
  std::unique_ptr<Stmt> falseBranch;

  if (match(TokenType::Else)) {
    falseBranch = statement();
  }

  return std::make_unique<IfStmt>(line, col, filename, cond, trueBranch,
                                  falseBranch);
}

std::unique_ptr<Stmt> Parser::whileStmt() {
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  auto condition = expression();
  auto body = statement();
  return std::make_unique<WhileStmt>(line, col, filename, condition, body);
}

std::unique_ptr<Stmt> Parser::loopStmt() {
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  std::unique_ptr<Expr> cond = std::make_unique<LiteralExpr>(
      line, col, filename,
      std::make_shared<const Token>(TokenType::True, "true", previous->line,
                                    previous->column, previous->filename));
  return std::make_unique<WhileStmt>(line, col, filename, cond, statement());
}

std::unique_ptr<Stmt> Parser::forStmt() {
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  consume("Expected '(' at the beginning of for statement but instead got '" +
              type2str(current->type) + "'.",
          TokenType::LeftParen);

  std::unique_ptr<Stmt> initializer;
  if (match(TokenType::Let)) {
    initializer = varDecl();
  } else if (!match(TokenType::Semicolon)) {
    initializer = expressionStmt();
  }

  std::unique_ptr<Expr> cond;
  if (!check(TokenType::Semicolon)) {
    cond = expression();
  }
  consume("Expected ';' after loop condition but instead got '" +
              type2str(current->type) + "'.",
          TokenType::Semicolon);

  std::unique_ptr<Expr> increment;
  if (!check(TokenType::RightParen)) {
    increment = expression();
  }
  consume("Expected ')' after loop clauses but instead got '" +
              type2str(current->type) + "'.",
          TokenType::RightParen);

  std::unique_ptr<Stmt> body = statement();

  if (!cond) {
    cond = std::make_unique<LiteralExpr>(
        line, col, filename,
        std::make_shared<const Token>(TokenType::True, "true", previous->line,
                                      previous->column, previous->filename));
  }

  return std::make_unique<ForStmt>(line, col, filename, initializer, cond,
                                   increment, body);
}

std::unique_ptr<Stmt> Parser::returnStmt() {
  std::unique_ptr<Expr> value;
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  if (!check(TokenType::Semicolon)) {
    value = expression();
  }
  CONSUME_SEMICOLON("return");

  return std::make_unique<ReturnStmt>(line, col, filename, value);
}

std::unique_ptr<Stmt> Parser::expressionStmt() {
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  auto expr = std::make_unique<ExprStmt>(line, col, filename, expression());
  CONSUME_SEMICOLON("expression");

  return expr;
}

std::unique_ptr<Expr> Parser::expression() {
  if (match(TokenType::Break)) {
    return std::make_unique<BreakExpr>(current->line, current->column,
                                       current->filename);
  } else if (match(TokenType::Continue)) {
    return std::make_unique<ContinueExpr>(current->line, current->column,
                                          current->filename);
  }

  return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
  std::unique_ptr<Expr> left = orExpr();
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  if (match(TokenType::Equal)) {
    std::unique_ptr<Expr> value = assignment();
    if (auto var = dynamic_cast<VariableExpr *>(left.get())) {
      return std::make_unique<AssignExpr>(line, col, filename, var->variable,
                                          value);
    } else if (auto get = dynamic_cast<GetFieldExpr *>(left.get())) {
      return std::make_unique<SetFieldExpr>(line, col, filename, get->instance,
                                            get->field, value);
    }

    throw SyntaxError(value.get(), "Invalid assignment value.");
  } else if (match(TokenType::Question)) {
    std::unique_ptr<Expr> trueExpr = assignment();
    consume("Expected ':' between ternary expressions but instead got '" +
                type2str(current->type) + "'.",
            TokenType::Colon);

    return std::make_unique<TernaryExpr>(line, col, filename, left, trueExpr,
                                         assignment());
  }

  return left;
}

std::unique_ptr<Expr> Parser::orExpr() {
  std::unique_ptr<Expr> left = andExpr();
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  while (match(TokenType::Or)) {
    std::shared_ptr<const Token> op = previous;

    left =
        std::make_unique<BinaryExpr>(line, col, filename, left, andExpr(), op);
  }

  return left;
}

std::unique_ptr<Expr> Parser::andExpr() {
  std::unique_ptr<Expr> left = equality();
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  while (match(TokenType::And)) {
    std::shared_ptr<const Token> op = previous;

    left =
        std::make_unique<BinaryExpr>(line, col, filename, left, equality(), op);
  }

  return left;
}

std::unique_ptr<Expr> Parser::equality() {
  std::unique_ptr<Expr> left = comparison();
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  while (match(TokenType::EqualEqual, TokenType::BangEqual)) {
    std::shared_ptr<const Token> op = previous;

    left = std::make_unique<BinaryExpr>(line, col, filename, left, comparison(),
                                        op);
  }

  return left;
}

std::unique_ptr<Expr> Parser::comparison() {
  std::unique_ptr<Expr> left = term();
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  while (match(TokenType::Greater, TokenType::GreaterEqual, TokenType::Less,
               TokenType::LessEqual)) {
    std::shared_ptr<const Token> op = previous;

    left = std::make_unique<BinaryExpr>(line, col, filename, left, term(), op);
  }

  return left;
}

std::unique_ptr<Expr> Parser::term() {
  std::unique_ptr<Expr> left = factor();
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  while (match(TokenType::Plus, TokenType::Minus, TokenType::ModOp)) {
    std::shared_ptr<const Token> op = previous;

    left =
        std::make_unique<BinaryExpr>(line, col, filename, left, factor(), op);
  }

  return left;
}

std::unique_ptr<Expr> Parser::factor() {
  std::unique_ptr<Expr> left = unary();
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  while (match(TokenType::Star, TokenType::Slash)) {
    std::shared_ptr<const Token> op = previous;

    left = std::make_unique<BinaryExpr>(line, col, filename, left, unary(), op);
  }

  return left;
}

std::unique_ptr<Expr> Parser::unary() {
  if (match(TokenType::Bang, TokenType::Minus)) {
    std::shared_ptr<const Token> op = previous;
    return std::make_unique<UnaryExpr>(current->line, current->column,
                                       current->filename, unary(), op);
  } else {
    return call();
  }
}

std::unique_ptr<Expr> Parser::call() {
  std::unique_ptr<Expr> expr = primary();
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  while (1) {
    if (match(TokenType::LeftParen)) {
      std::vector<std::unique_ptr<Expr>> args;
      if (!check(TokenType::RightParen)) {
        do {
          args.emplace_back(expression());
        } while (match(TokenType::Comma));
      }

      consume("Expected ')' after function call but instead got '" +
                  type2str(current->type) + "'.",
              TokenType::RightParen);
      expr = std::make_unique<FnCallExpr>(line, col, filename, expr, args);
    } else if (match(TokenType::Dot)) {
      std::shared_ptr<const Token> prop =
          consume("Expected class property or method but instead got '" +
                      type2str(current->type) + "'.",
                  TokenType::Identifier);
      expr = std::make_unique<GetFieldExpr>(line, col, filename, expr, prop);
    } else {
      break;
    }
  }

  return expr;
}

std::unique_ptr<Expr> Parser::primary() {
  int line = current->line, col = current->column;
  const char *filename = current->filename;

  if (match(TokenType::Super)) {
    consume("Expected '.' after super keyword.", TokenType::Dot);
    std::shared_ptr<const Token> field =
        consume("Expected superclass field.", TokenType::Identifier);

    return std::make_unique<SuperExpr>(line, col, filename, field);
  } else if (match(TokenType::This)) {
    return std::make_unique<ThisExpr>(line, col, filename, previous);
  } else if (match(TokenType::Identifier)) {
    return std::make_unique<VariableExpr>(line, col, filename, previous);
  } else if (match(TokenType::Number, TokenType::Char, TokenType::String,
                   TokenType::True, TokenType::False, TokenType::Nil)) {
    return std::make_unique<LiteralExpr>(line, col, filename, previous);
  } else if (match(TokenType::LeftParen)) {
    std::unique_ptr<Expr> expr = expression();
    consume("Missing closing parenthesis ')' after expression.",
            TokenType::RightParen);
    return std::make_unique<GroupingExpr>(line, col, filename, expr);
  }

  throw SyntaxError(current.get(), "'" + type2str(current->type) +
                                       "' not a valid expression statement.");
}

void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    if (previous->type == TokenType::Semicolon) {
      return;
    }

    switch (current->type) {
    case TokenType::Class:
    case TokenType::Fn:
    case TokenType::Let:
    case TokenType::While:
    case TokenType::Loop:
    case TokenType::For:
    case TokenType::If:
    case TokenType::Return:
      return;
    default:
      break;
    }

    advance();
  }
}

std::shared_ptr<const Token> Parser::advance() {
  previous = current;

  if (isAtEnd()) {
    return current;
  }

  current = Lexer::getNextToken(this->source);
  if (current->type == TokenType::Error) {
    throw SyntaxError(current.get(), std::get<const char *>(current->lexeme));
  } else {
    return previous;
  }
}

std::vector<std::unique_ptr<Stmt>> Parser::stmtSequence() {
  std::vector<std::unique_ptr<Stmt>> stmts;

  while (!check(TokenType::RightBrace) && !isAtEnd()) {
    try {
      stmts.push_back(declaration());
    } catch (SyntaxError &e) {
      hadError = true;
      std::cout << e.what();
      synchronize();
    }
  }

  consume("Expected '}' at scope block end but instead got '" +
              type2str(current->type) + "'.",
          TokenType::RightBrace);
  return stmts;
}

template <typename... TokenTypes>
bool Parser::match(const TokenTypes &...types) {
  if (!(check(types) || ...)) {
    return false;
  }

  advance();
  return true;
}

std::shared_ptr<const Token> Parser::consume(const std::string &msg,
                                             const TokenType &expected) {
  if (!check(expected)) {
    throw SyntaxError(current.get(), msg);
  }

  std::shared_ptr<const Token> res = current;
  advance();
  return res;
}

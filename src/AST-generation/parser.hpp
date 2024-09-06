#ifndef PARSER_H
#define PARSER_H

#include "expressions.hpp"
#include "lexer.hpp"
#include "statements.hpp"
#include "tokens/token_type.hpp"

#include <fstream>
#include <memory>
#include <unordered_set>
#include <vector>

#define CONSUME_SEMICOLON(keyword)                                             \
  consume("Expected ';' after " + std::string(keyword) + " statement.",        \
          TokenType::Semicolon);

class Parser {
public:
  Parser(std::ifstream &file, const char *filename)
      : source(Lexer::Source(file, filename)),
        current(Lexer::getNextToken(this->source)), previous(current),
        hadError(false) {
    importedFiles.insert(filename);
  }

  Parser(std::ifstream &file, const char *filename,
         std::unordered_set<std::string> &importedFiles)
      : importedFiles(importedFiles), source(Lexer::Source(file, filename)),
        current(Lexer::getNextToken(this->source)), previous(current),
        hadError(false) {}

  std::vector<std::unique_ptr<Stmt>> parse();

private:
  void synchronize();
  std::shared_ptr<const Token> advance();

  inline bool isAtEnd() { return current->type == TokenType::Eof; }

  template <typename... TokenTypes>
  inline bool check(const TokenTypes &...types) {
    return ((current->type == types) || ...);
  }

  template <typename... TokenTypes> bool match(const TokenTypes &...types);
  std::shared_ptr<const Token> consume(const std::string &msg,
                                       const TokenType &expected);

  std::vector<std::unique_ptr<Stmt>> importStmt();
  std::unique_ptr<FnStmt> functionDecl(const std::string &);
  std::unique_ptr<VarStmt> varDecl();
  std::unique_ptr<ClassStmt> classDecl();

  std::vector<std::unique_ptr<Stmt>> stmtSequence();

  std::unique_ptr<Stmt> declaration();
  std::unique_ptr<Stmt> statement();
  std::unique_ptr<Stmt> ifStmt();
  std::unique_ptr<Stmt> whileStmt();
  std::unique_ptr<Stmt> loopStmt();
  std::unique_ptr<Stmt> forStmt();
  std::unique_ptr<Stmt> returnStmt();
  std::unique_ptr<Stmt> scopedStmt();
  std::unique_ptr<Stmt> expressionStmt();

  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> assignment();
  std::unique_ptr<Expr> orExpr();
  std::unique_ptr<Expr> andExpr();
  std::unique_ptr<Expr> equality();
  std::unique_ptr<Expr> comparison();
  std::unique_ptr<Expr> term();
  std::unique_ptr<Expr> factor();
  std::unique_ptr<Expr> unary();
  std::unique_ptr<Expr> call();
  std::unique_ptr<Expr> primary();

private:
  Lexer::Source source;
  std::shared_ptr<const Token> current;
  std::shared_ptr<const Token> previous;
  std::unordered_set<std::string> importedFiles;

public:
  bool hadError;
};

#endif

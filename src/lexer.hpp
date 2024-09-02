#ifndef LEXER_H
#define LEXER_H

#include "token.hpp"
#include "token_type.hpp"

#include <string>
#include <memory>

class Lexer {
private:
  int line;
  std::string filename;
  const char *start;
  const char *current;

public:
  bool hadError;

private:
  void skipWhitespace();
  void goToNextLine();

  std::shared_ptr<const Token> makeToken(TokenType, std::string = "");
  std::shared_ptr<const Token> makeNumberToken();
  std::shared_ptr<const Token> makeIdentifierToken();
  std::shared_ptr<const Token> makeErrorToken(std::string);

  TokenType isIdentifierOrKeywork();
  TokenType checkKeyword(int startIndex, const std::string &restOfKeyword,
                         TokenType typeIfMatch);

  char advance();
  char peek() const;
  char peekNext() const;

  bool isAtEnd() const;
  bool isDigit(char ch) const;
  bool isAlpha(char ch) const;
  bool match(char ch);

public:
  Lexer(const char *, const std::string &);

  int getLine();
  int getColumn();
  std::string getFilename();

  std::shared_ptr<const Token> getNextToken();
};

#endif

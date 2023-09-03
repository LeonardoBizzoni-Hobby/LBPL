#ifndef LEXER_H
#define LEXER_H

#include "common.h"

#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>

class Lexer {
private:
  int line;
  std::ifstream &stream;
  std::string filename;
  std::string currentLine;
  std::string::iterator start;
  std::string::iterator current;

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
  bool isAtLineEnd() const;
  bool isDigit(char ch) const;
  bool isAlpha(char ch) const;
  bool match(char ch);

public:
  Lexer(std::ifstream &, const std::string &);

  int getLine();
  int getColumn();
  std::string getFilename();

  std::shared_ptr<const Token> getNextToken();
};

#endif

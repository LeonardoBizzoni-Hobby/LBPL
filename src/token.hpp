#ifndef TOKEN_H
#define TOKEN_H

#include "token_type.hpp"

#include <string>

class Token {
public:
  int line;
  int column;
  TokenType type;
  std::string lexeme;
  std::string filename;

  Token(TokenType type, const std::string &lexeme, int line, int column, const std::string &filename)
      : type(type), lexeme(lexeme), line(line), column(column), filename(filename) {}
};

#endif

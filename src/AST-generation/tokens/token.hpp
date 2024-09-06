#ifndef TOKEN_H
#define TOKEN_H

#include "token_type.hpp"

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <variant>

using literal_t =
    std::variant<const char *, char, int32_t, float, std::nullptr_t>;

struct Token {
  Token(TokenType type, literal_t lexeme, int32_t line, int32_t column,
        const char *filename)
      : type(type), lexeme(lexeme), line(line), column(column),
        filename(filename) {}

public:
  int32_t line;
  int32_t column;
  TokenType type;
  literal_t lexeme;
  const char *filename;
};

std::ostream &operator<<(std::ostream &os, const Token tk);

#endif

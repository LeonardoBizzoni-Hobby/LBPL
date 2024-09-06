#ifndef LEXER_H
#define LEXER_H

#include "tokens/token.hpp"
#include "tokens/token_type.hpp"

#include <cstdint>
#include <fstream>
#include <memory>

namespace Lexer {
struct Source {
  Source(std::ifstream &file, const char *filepath)
      : stream(std::move(file)), filepath(filepath), line(1), column(0) {}

  inline char advance() {
    char res = this->stream.get();
    ++column;
    return res;
  }

public:
  std::ifstream stream;
  uint64_t line, column;
  const char *filepath;
};

std::shared_ptr<const Token> getNextToken(Source &file);

std::shared_ptr<const Token> makeIdentifierToken(Source &file);
std::shared_ptr<const Token> makeNumberToken(Source &file);

void skipWhitespace(Source &file);

TokenType isIdentifierOrKeyword(const char *lexeme, size_t len);
TokenType checkKeyword(const char *token, size_t token_len, int startIndex,
                       const std::string_view &restOfKeyword,
                       TokenType typeIfMatch);

inline constexpr bool isDigit(char ch) { return ch >= '0' && ch <= '9'; }
inline constexpr bool isAlpha(char ch) {
  return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_' ||
         ch < -1;
}
} // namespace Lexer

// class Lexer {
// private:
//   int line;
//   std::string filename;
//   const char *start;
//   const char *current;

// public:
//   bool hadError;

// private:
//   void skipWhitespace();
//   void goToNextLine();

//   std::shared_ptr<const Token> makeToken(TokenType, std::string = "");
//   std::shared_ptr<const Token> makeNumberToken();
//   std::shared_ptr<const Token> makeIdentifierToken();
//   std::shared_ptr<const Token> makeErrorToken(std::string);

//   TokenType isIdentifierOrKeywork();
//   TokenType checkKeyword(int startIndex, const std::string &restOfKeyword,
//                          TokenType typeIfMatch);

//   char advance();
//   char peek() const;
//   char peekNext() const;

//   bool isAtEnd() const;
//   bool isDigit(char ch) const;
//   bool isAlpha(char ch) const;
//   bool match(char ch);

// public:
//   Lexer(std::ifstream &, const std::string &);

//   int getLine();
//   int getColumn();
//   std::string getFilename();

//   std::shared_ptr<const Token> getNextToken();
// };

#endif

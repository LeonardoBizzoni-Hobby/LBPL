#include "lexer.hpp"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <string_view>

#define MAKE_TOKEN(TYPE, LEXEME)                                               \
  std::make_shared<const Token>(TYPE, LEXEME, file.line, file.column,          \
                                file.filepath)

namespace Lexer {
std::shared_ptr<const Token> getNextToken(Source &file) {
  if (file.stream.peek() == EOF) {
    return MAKE_TOKEN(TokenType::Eof, 0);
  }

  skipWhitespace(file);
  char ch = file.stream.peek();

  if (isDigit(ch)) {
    return makeNumberToken(file);
  } else if (isAlpha(ch)) {
    return makeIdentifierToken(file);
  }

  switch ((ch = file.advance())) {
  case '(':
    return MAKE_TOKEN(TokenType::LeftParen, 0);
  case ')':
    return MAKE_TOKEN(TokenType::RightParen, 0);
  case '{':
    return MAKE_TOKEN(TokenType::LeftBrace, 0);
  case '}':
    return MAKE_TOKEN(TokenType::RightBrace, 0);
  case '?':
    return MAKE_TOKEN(TokenType::Question, 0);
  case ',':
    return MAKE_TOKEN(TokenType::Comma, 0);
  case '.':
    return MAKE_TOKEN(TokenType::Dot, 0);
  case ':':
    return MAKE_TOKEN(TokenType::Colon, 0);
  case ';':
    return MAKE_TOKEN(TokenType::Semicolon, 0);
  case '%':
    return MAKE_TOKEN(TokenType::ModOp, 0);
  case '&':
    if (char _ch = file.stream.peek(); _ch == '&') {
      file.advance();
      return MAKE_TOKEN(TokenType::And, 0);
    } else {
      std::string str_msg = "invalid token '" + std::string(1, _ch) + "'";
      char *msg = (char *)malloc(str_msg.size() + 1);
      std::strncpy(msg, str_msg.c_str(), str_msg.size() + 1);
      return MAKE_TOKEN(TokenType::Error, msg);
    }
  case '|':
    if (char _ch = file.stream.peek(); _ch == '|') {
      file.advance();
      return MAKE_TOKEN(TokenType::Or, 0);
    } else {
      std::string str_msg = "invalid token '" + std::string(1, _ch) + "'";
      char *msg = (char *)malloc(str_msg.size() + 1);
      std::strncpy(msg, str_msg.c_str(), str_msg.size() + 1);
      return MAKE_TOKEN(TokenType::Error, msg);
    }
  case '-':
    return MAKE_TOKEN(TokenType::Minus, 0);
  case '+':
    return MAKE_TOKEN(TokenType::Plus, 0);
  case '/':
    return MAKE_TOKEN(TokenType::Slash, 0);
  case '*':
    return MAKE_TOKEN(TokenType::Star, 0);
  case '!':
    if (file.stream.peek() == '=') {
      file.advance();
      return MAKE_TOKEN(TokenType::BangEqual, 0);
    } else {
      return MAKE_TOKEN(TokenType::Bang, 0);
    }
  case '=':
    if (file.stream.peek() == '=') {
      file.advance();
      return MAKE_TOKEN(TokenType::EqualEqual, 0);
    } else {
      return MAKE_TOKEN(TokenType::Equal, 0);
    }
  case '>':
    if (file.stream.peek() == '>') {
      file.advance();
      return MAKE_TOKEN(TokenType::ShiftRight, 0);
    } else if (file.stream.peek() == '=') {
      file.advance();
      return MAKE_TOKEN(TokenType::GreaterEqual, 0);
    } else {
      return MAKE_TOKEN(TokenType::Greater, 0);
    }
  case '<':
    if (file.stream.peek() == '<') {
      file.advance();
      return MAKE_TOKEN(TokenType::ShiftLeft, 0);
    } else if (file.stream.peek() == '=') {
      file.advance();
      return MAKE_TOKEN(TokenType::LessEqual, 0);
    } else {
      return MAKE_TOKEN(TokenType::Less, 0);
    }
  case '\'': {
    char lexeme = file.advance();
    if (file.stream.peek() != '\'') {
      return MAKE_TOKEN(TokenType::Error, "A char must be one character long.");
    } else {
      return MAKE_TOKEN(TokenType::Char, lexeme);
    }
  }
  case '"': {
    size_t len = 0;
    char *lexeme = 0;
    char new_ch = 0;

    while (file.stream.peek() != EOF && file.stream.peek() != '"') {
      if (file.stream.peek() == '\\') {
        file.advance();

        switch (file.stream.peek()) {
        case 'n':
          new_ch = '\n';
          break;
        case 't':
          new_ch = '\t';
          break;
        case 'r':
          new_ch = '\r';
          break;
        case '\'':
          new_ch = '\'';
          break;
        case '\"':
          new_ch = '"';
          break;
        case '\\':
          new_ch = '\\';
          break;
        default:
          return MAKE_TOKEN(TokenType::Error, "invalid escape sequence");
        }

        file.advance();
      } else {
        new_ch = file.advance();
      }

      lexeme = (char *)realloc(lexeme, ++len);
      lexeme[len - 1] = new_ch;
    }

    if (file.stream.peek() == EOF) {
      return MAKE_TOKEN(TokenType::Error, "Unterminated string.");
    } else {
      file.advance();
      return MAKE_TOKEN(TokenType::String, lexeme);
    }
  }
  }

  if (file.stream.eof()) {
    return MAKE_TOKEN(TokenType::Eof, 0);
  }

  return MAKE_TOKEN(TokenType::Error, "\033[1;36mHow did you get here?\033[0m");
}

void skipWhitespace(Source &file) {
  while (!file.stream.eof()) {
    switch (file.stream.peek()) {
    case '\r':
    case '\n': {
      file.line++;
      file.advance();
      file.column = 0;
    } break;
    case ' ': {
      file.advance();
    } break;
    case '\t': {
      file.column += 3;
      file.advance();
    } break;
    case '#': {
      while (file.stream.peek() != '\n' || file.stream.peek() != '\r') {
        file.advance();
      }
    } break;
    default:
      return;
    }
  }
}

std::shared_ptr<const Token> makeNumberToken(Source &file) {
  size_t size = 0;
  char *strlexeme = 0;
  bool is_float = false;

  while (isDigit(file.stream.peek())) {
    strlexeme = (char *)realloc(strlexeme, ++size);
    strlexeme[size - 1] = file.advance();
  }

  if (file.stream.peek() == '.') {
    is_float = true;
    strlexeme = (char *)realloc(strlexeme, ++size);
    strlexeme[size - 1] = file.advance();

    if (isDigit(file.stream.peek())) {
      strlexeme = (char *)realloc(strlexeme, ++size);
      strlexeme[size - 1] = file.advance();

      while (isDigit(file.stream.peek())) {
        strlexeme = (char *)realloc(strlexeme, ++size);
        strlexeme[size - 1] = file.advance();
      }
    }
  }

  strlexeme = (char *)realloc(strlexeme, ++size);
  strlexeme[size - 1] = 0;

  return MAKE_TOKEN(TokenType::Number,
                    (is_float ? std::stof(strlexeme) : std::stoi(strlexeme)));
}

std::shared_ptr<const Token> makeIdentifierToken(Source &file) {
  size_t size = 0;
  char *strlexeme = 0;

  while (isAlpha(file.stream.peek()) || isDigit(file.stream.peek())) {
    strlexeme = (char *)realloc(strlexeme, ++size);
    strlexeme[size - 1] = file.advance();
  }

  strlexeme = (char *)realloc(strlexeme, ++size);
  strlexeme[size - 1] = 0;

  return MAKE_TOKEN(isIdentifierOrKeyword(strlexeme, size), strlexeme);
}

TokenType isIdentifierOrKeyword(const char *lexeme, size_t len) {
  switch (lexeme[0]) {
  case 'b':
    return checkKeyword(lexeme, len, 1, "reak", TokenType::Break);
  case 'c':
    if (len > 1) {
      switch (lexeme[1]) {
      case 'l':
        return checkKeyword(lexeme, len, 2, "ass", TokenType::Class);
      case 'o':
        return checkKeyword(lexeme, len, 2, "ntinue", TokenType::Continue);
      }
    }

    break;
  case 'e':
    return checkKeyword(lexeme, len, 1, "lse", TokenType::Else);
  case 'f':
    if (len > 1) {
      switch (lexeme[1]) {
      case 'a':
        return checkKeyword(lexeme, len, 2, "lse", TokenType::False);
      case 'o':
        return checkKeyword(lexeme, len, 2, "r", TokenType::For);
      case 'n':
        return checkKeyword(lexeme, len, 2, "", TokenType::Fn);
      }
    }

    break;
  case 'i':
    if (len > 1) {
      switch (lexeme[1]) {
      case 'f':
        return checkKeyword(lexeme, len, 2, "", TokenType::If);
      case 'm':
        return checkKeyword(lexeme, len, 2, "port", TokenType::Import);
      }
    }

    break;
  case 'l':
    if (len > 1) {
      switch (lexeme[1]) {
      case 'e':
        return checkKeyword(lexeme, len, 2, "t", TokenType::Let);
      case 'o':
        return checkKeyword(lexeme, len, 2, "op", TokenType::Loop);
      }
    }
    break;
  case 'n':
    return checkKeyword(lexeme, len, 1, "il", TokenType::Nil);
  case 'r':
    return checkKeyword(lexeme, len, 1, "eturn", TokenType::Return);
  case 's':
    return checkKeyword(lexeme, len, 1, "uper", TokenType::Super);
  case 't':
    if (len > 1) {
      switch (lexeme[1]) {
      case 'r':
        return checkKeyword(lexeme, len, 2, "ue", TokenType::True);
      case 'h':
        return checkKeyword(lexeme, len, 2, "is", TokenType::This);
      }
    }
    break;
  case 'w':
    return checkKeyword(lexeme, len, 1, "hile", TokenType::While);
  }

  return TokenType::Identifier;
}

TokenType checkKeyword(const char *token, size_t token_len, int startIndex,
                       const std::string_view &restOfKeyword,
                       TokenType typeIfMatch) {
  if ((token_len - startIndex) != (restOfKeyword.length() + 1)) {
    return TokenType::Identifier;
  }

  if (strcmp(token + startIndex, restOfKeyword.data())) {
    return TokenType::Identifier;
  }

  return typeIfMatch;
}
} // namespace Lexer

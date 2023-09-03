#include "lexer.h"

Lexer::Lexer(std::ifstream &stream, const std::string &filename)
    : stream(stream), line(1), hadError(false), filename(filename) {
  goToNextLine();
}

bool Lexer::isAtEnd() const { return stream.eof(); }
bool Lexer::isAtLineEnd() const { return *current == '\0'; }
bool Lexer::isDigit(char ch) const { return ch >= '0' && ch <= '9'; }
bool Lexer::isAlpha(char ch) const {
  return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_';
}
bool Lexer::match(char ch) {
  if (isAtLineEnd() || ch != *current) {
    return false;
  }

  advance();
  return true;
}

void Lexer::goToNextLine() {
  std::getline(stream, currentLine);
  current = currentLine.begin();
  start = currentLine.begin();
}

char Lexer::peek() const { return *current; }
char Lexer::peekNext() const { return *(current + 1); }
char Lexer::advance() {
  if (isAtLineEnd()) {
    if (isAtEnd()) {
      return '\0';
    }

    std::getline(stream, currentLine);
    current = currentLine.begin();

    return *current;
  }

  return *(current++);
}

TokenType Lexer::checkKeyword(int startIndex, const std::string &restOfKeyword,
                              TokenType typeIfMatch) {
  if (current - (start + startIndex) != restOfKeyword.length()) {
    return TokenType::Identifier;
  }

  for (int i = 0; i < restOfKeyword.length(); i++) {
    if (*(start + startIndex + i) != restOfKeyword[i]) {
      return TokenType::Identifier;
    }
  }

  return typeIfMatch;
}

TokenType Lexer::isIdentifierOrKeywork() {
  switch (*start) {
  case 'b':
    return checkKeyword(1, "reak", TokenType::Break);
  case 'c':
    if (current - start > 1) {
      switch (*(start + 1)) {
      case 'l':
        return checkKeyword(2, "ass", TokenType::Class);
      case 'o':
        return checkKeyword(2, "ntinue", TokenType::Continue);
      }
    }

    break;
  case 'e':
    return checkKeyword(1, "lse", TokenType::Else);
  case 'f':
    if (current - start > 1) {
      switch (*(start + 1)) {
      case 'a':
        return checkKeyword(2, "lse", TokenType::False);
      case 'o':
        return checkKeyword(2, "r", TokenType::For);
      case 'n':
        return checkKeyword(2, "", TokenType::Fn);
      }
    }

    break;
  case 'i':
    if (current - start > 1) {
      switch (*(start + 1)) {
      case 'f':
        return checkKeyword(2, "", TokenType::If);
      case 'm':
        return checkKeyword(2, "port", TokenType::Import);
      }
    }

    break;
  case 'l':
    if (current - start > 1) {
      switch (*(start + 1)) {
      case 'e':
        return checkKeyword(2, "t", TokenType::Let);
      case 'o':
        return checkKeyword(2, "op", TokenType::Loop);
      }
    }
    break;
  case 'n':
    return checkKeyword(1, "il", TokenType::Nil);
  case 'r':
    return checkKeyword(1, "eturn", TokenType::Return);
  case 's':
    return checkKeyword(1, "uper", TokenType::Super);
  case 't':
    if (current - start > 1) {
      switch (*(start + 1)) {
      case 'r':
        return checkKeyword(2, "ue", TokenType::True);
      case 'h':
        return checkKeyword(2, "is", TokenType::This);
      }
    }
    break;
  case 'w':
    return checkKeyword(1, "hile", TokenType::While);
  }

  return TokenType::Identifier;
}

std::shared_ptr<const Token> Lexer::makeToken(TokenType type,
                                              std::string value) {
  if (value != "") {
    return std::make_shared<const Token>(type, value, line,
                                         start - currentLine.begin(), filename);
  }

  return std::make_shared<const Token>(type, std::string(start, current), line,
                                       start - currentLine.begin(), filename);
}

std::shared_ptr<const Token> Lexer::makeNumberToken() {
  while (isDigit(peek())) {
    advance();
  }

  if (peek() == '.' && isDigit(peekNext())) {
    advance();
    while (isDigit(peek())) {
      advance();
    }
  }

  return makeToken(TokenType::Number);
}

std::shared_ptr<const Token> Lexer::makeIdentifierToken() {
  while (isAlpha(peek()) || isDigit(peek()) || peek() < 0) {
    advance();
  }

  return makeToken(isIdentifierOrKeywork());
}

std::shared_ptr<const Token> Lexer::makeErrorToken(std::string msg) {
  hadError = true;
  return std::make_shared<const Token>(TokenType::Error, msg, line,
                                       start - currentLine.begin(), filename);
}

void Lexer::skipWhitespace() {
  do {
    switch (peek()) {
    case '\0':
      line++;
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '#':
      while (!isAtLineEnd()) {
        advance();
      }

      break;
    default:
      return;
    }
  } while (!isAtEnd());
}

std::shared_ptr<const Token> Lexer::getNextToken() {
  skipWhitespace();
  start = current;

  char ch = advance();

  if (isDigit(ch)) {
    return makeNumberToken();
  } else if (isAlpha(ch) || (int)ch < 0) {
    return makeIdentifierToken();
  }

  switch (ch) {
  case '(':
    return makeToken(TokenType::LeftParen);
  case ')':
    return makeToken(TokenType::RightParen);
  case '{':
    return makeToken(TokenType::LeftBrace);
  case '}':
    return makeToken(TokenType::RightBrace);
  case '?':
    return makeToken(TokenType::Question);
  case ',':
    return makeToken(TokenType::Comma);
  case '.':
    return makeToken(TokenType::Dot);
  case ':':
    return makeToken(TokenType::Colon);
  case ';':
    return makeToken(TokenType::Semicolon);
  case '%':
    return makeToken(TokenType::ModOp);
  case '&':
    if (match('&')) {
      return makeToken(TokenType::And);
    }
    return makeErrorToken("Invalid token '" + std::to_string(*current) + "'.");
  case '|':
    if (match('|')) {
      return makeToken(TokenType::Or);
    }
    return makeErrorToken("Invalid operator '|' in: `" + currentLine +
                          "`.\n\tDid you mean '|\033[4;32m|\033[0m'?");
  case '-':
    return makeToken(TokenType::Minus);
  case '+':
    return makeToken(TokenType::Plus);
  case '/':
    return makeToken(TokenType::Slash);
  case '*':
    return makeToken(TokenType::Star);
  case '!':
    if (match('=')) {
      return makeToken(TokenType::BangEqual);
    }
    return makeToken(TokenType::Bang);
  case '=':
    if (match('=')) {
      return makeToken(TokenType::EqualEqual);
    }
    return makeToken(TokenType::Equal);
  case '>':
    if (match('>')) {
      return makeToken(TokenType::ShiftRight);
    }
    if (match('=')) {
      return makeToken(TokenType::GreaterEqual);
    }
    return makeToken(TokenType::Greater);
  case '<':
    if (match('<')) {
      return makeToken(TokenType::ShiftLeft);
    }
    if (match('=')) {
      return makeToken(TokenType::LessEqual);
    }
    return makeToken(TokenType::Less);
  case '\'': {
    std::string val(1, advance());
    if (!match('\'')) {
      auto res =
          makeErrorToken("A char can't be more then one character long.");
      goToNextLine();
      return res;
    }
    return makeToken(TokenType::Char, val);
  }
  case '"': {
    std::string lexeme;

    while (peek() != '"' && !isAtLineEnd()) {
      if (peek() == '\\') {
        advance();

        switch (peek()) {
        case 'n':
          lexeme += "\n";
          break;
        case 't':
          lexeme += "\t";
          break;
        case 'r':
          lexeme += "\r";
          break;
        case '\'':
          lexeme += "'";
          break;
        case '\"':
          lexeme += "\"";
          break;
        case '\\':
          lexeme += "\\";
          break;
        }

        advance();
      } else {
        lexeme += advance();
      }
    }

    if (isAtLineEnd()) {
      return makeErrorToken("Unterminated string.");
    }

    advance();
    return makeToken(TokenType::String, lexeme);
  }
  }

  if (isAtEnd()) {
    return makeToken(TokenType::Eof, "EoF");
  }

  return makeErrorToken("\033[1;36mHow did you get here?\033[0m");
}

int Lexer::getLine() { return line; }
int Lexer::getColumn() { return current - currentLine.begin(); }
std::string Lexer::getFilename() { return filename; }

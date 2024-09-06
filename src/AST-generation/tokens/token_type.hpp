#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <utility>

enum class TokenType {
  // Single-character tokens.
  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,
  Question,
  Comma,
  Dot,
  Colon,
  Semicolon,
  ModOp,
  Minus,
  Plus,
  Slash,
  Star,

  // One or two character tokens.
  Bang,
  BangEqual,
  Equal,
  EqualEqual,
  ShiftRight,
  Greater,
  GreaterEqual,
  ShiftLeft,
  Less,
  LessEqual,

  // Literals.
  Identifier,
  String,
  Char,
  Number,

  // Keywords.
  Import,
  Let,
  And,
  Class,
  Else,
  False,
  Fn,
  For,
  If,
  Nil,
  Or,
  Return,
  Break,
  Continue,
  Super,
  This,
  True,
  While,
  Loop,
  Main,
  Eof,
  Error,
};

static constexpr std::array<std::pair<TokenType, const char *>, 50>
    type2str_map = {{
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"},
        {TokenType::Question, "?"},
        {TokenType::Comma, ","},
        {TokenType::Dot, "."},
        {TokenType::Colon, ","},
        {TokenType::Semicolon, ";"},
        {TokenType::ModOp, "%"},
        {TokenType::Minus, "-"},
        {TokenType::Plus, "+"},
        {TokenType::Slash, "/"},
        {TokenType::Star, "*"},

        {TokenType::Bang, "!"},
        {TokenType::BangEqual, "!="},
        {TokenType::Equal, "="},
        {TokenType::EqualEqual, "=="},
        {TokenType::ShiftRight, ">>"},
        {TokenType::Greater, ">"},
        {TokenType::GreaterEqual, ">="},
        {TokenType::ShiftLeft, "<<"},
        {TokenType::Less, "<"},
        {TokenType::LessEqual, "<="},

        {TokenType::Identifier, "Identifier"},
        {TokenType::String, "String"},
        {TokenType::Char, "Char"},
        {TokenType::Number, "Number"},

        {TokenType::Import, "import"},
        {TokenType::Let, "let"},
        {TokenType::And, "and"},
        {TokenType::Class, "class"},
        {TokenType::Else, "else"},
        {TokenType::False, "false"},
        {TokenType::Fn, "fn"},
        {TokenType::For, "for"},
        {TokenType::If, "if"},
        {TokenType::Nil, "nil"},
        {TokenType::Or, "or"},
        {TokenType::Return, "return"},
        {TokenType::Break, "break"},
        {TokenType::Continue, "continue"},
        {TokenType::Super, "super"},
        {TokenType::This, "this"},
        {TokenType::True, "true"},
        {TokenType::While, "while"},
        {TokenType::Loop, "loop"},
        {TokenType::Main, "main"},
        {TokenType::Eof, "eof"},
        {TokenType::Error, "error"},
    }};

inline std::string type2str(TokenType type) {
  const auto it =
      std::find_if(type2str_map.begin(), type2str_map.end(),
                   [&](const auto &value) { return value.first == type; });

  if (it != type2str_map.end()) {
    return it->second;
  } else {
    __builtin_unreachable();
  }
}

#endif

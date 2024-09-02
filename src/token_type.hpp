#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

enum TokenType {
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

#endif

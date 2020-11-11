#pragma once
#include "reader.h"
#include <iostream>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>
#include <string>

enum class OpType {
  ASSIGN,     // 赋值
  ADD,        //加法
  INC,        //自增
  ADD_ASSIGN, //加赋值
  SUB,        //减
  DEC,        //自减
  SUB_ASSIGN, //减赋值
  MUL_ASSIGN,
  DIV,
  DIV_ASSIGN,
  MOD,
  MOD_ASSIGN,
  BITWISE_AND_ASSIGN,
  BITWISE_OR,
  BITWISE_OR_ASSIGN,
  BITWISE_XOR,
  BITWISE_XOR_ASSIGN,
  BITWISE_NOT,
  AND,
  AND_ASSIGN,
  OR,
  OR_ASSIGN,
  NOT,
  SHL,
  SHL_ASSIGN,
  SHR,
  SHR_ASSIGN,
  LESS,
  LESS_EQUAL,
  EQUAL,
  INEQUAL,
  GREATER,
  GREATER_EQUAL,
  CONCAT,
  ASTERISK,
  AMPERSAND,
  QUESTION,
  COMMA,
  COLON,
  SEMICOLON,
  DOT,
  ARROW,
  L_BRACE,
  R_BRACE,
  L_SQUARE,
  R_SQUARE,
  L_PAREN,
  R_PAREN,
};

enum class ReservedWordType {
  CHAR,
  UNSIGNED,
  UNION,
  INT,
  SIGNER,
  TYPEDEF,
  LONG,
  CONST,
  SIZEOF,
  FLOAT,
  STATIC,
  IF,
  DOUBLE,
  EXTERN,
  ELSE,
  VOID,
  STRUCT,
};

class Token {
public:
  enum class TokenType {
    OP,
    ReservedWord,
    Ident,
    Number,
    String,
    Char,
    Null,
  };

  Token() {}
  Token(OpType op_type, Position pos = Position{});
  Token(ReservedWordType reserved_word, Position pos = Position{});
  Token(char *data, Token::TokenType type, Position pos = Position{});

  bool is_op() const;
  bool is_reserved_word() const;
  bool is_ident() const;
  bool is_number() const;
  bool is_string() const;
  bool is_char() const;
  bool is_null() const;

  OpType as_op() const;
  ReservedWordType as_reserved_word() const;
  char *as_ident() const;
  char *as_number() const;
  char *as_string() const;
  char *as_char() const;

  TokenType type() const;

  Position p_token;

  inline bool operator==(const Token &other) const {
    if (this->is_op() && other.is_op()) {
      return this->as_op() == other.as_op();
    } else if (this->is_reserved_word() && other.is_reserved_word()) {
      return this->as_reserved_word() == other.as_reserved_word();
    } else {
      return this->token_type == other.token_type;
    }
  }

  inline bool operator<(const Token &other) const {
    if (this->is_op() && other.is_op()) {
      return this->as_op() < other.as_op();
    } else if (this->is_reserved_word() && other.is_reserved_word()) {
      return this->as_reserved_word() < other.as_reserved_word();
    } else {
      return this->token_type < other.token_type;
    }
  }

private:
  union TokenValue {
    OpType op_type;
    ReservedWordType reserved_word;
    char *data;
  };

  TokenType token_type;

  TokenValue token_value;
};

namespace plog {
Record &operator<<(Record &record, const OpType &o);
Record &operator<<(Record &record, const ReservedWordType &r);
Record &operator<<(Record &record, const Token &t);
} // namespace plog

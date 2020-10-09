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
  Token(OpType op_type, Position pos);
  Token(ReservedWordType reserved_word, Position pos);
  Token(char *data, bool is_number, Position pos);

  bool is_op() const;
  bool is_reserved_word() const;
  bool is_ident() const;
  bool is_number() const;

  OpType as_op() const;
  ReservedWordType as_reserved_word() const;
  char *as_ident() const;
  char *as_number() const;

  Position p_token;

private:
  enum class TokenType {
    OP,
    ReservedWord,
    Ident,
    Number,
  };

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

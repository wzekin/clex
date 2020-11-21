#pragma once
#include "reader.h"
#include <fmt/core.h>
#include <fmt/format.h>
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
  Token(Token::TokenType type, char *data = NULL, Position pos = Position{});

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

template <> struct fmt::formatter<OpType> : formatter<string_view> {
  bool is_details = false;
  constexpr auto parse(format_parse_context &ctx) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'd')) {
      it++;
      is_details = true;
    }
    if (it != end && *it != '}')
      throw format_error("invalid format");
    return it;
  }
  template <typename FormatContext>
  auto format(const OpType &p, FormatContext &ctx) {
    string_view s = "unknown";
#define PROCESS_VAL(p, name, details)                                          \
  case (p):                                                                    \
    s = is_details ? fmt::format("{} '{}'", details, name) : name;             \
    break;
    switch (p) {
      PROCESS_VAL(OpType::ASSIGN, "=", "ASSIGN ");
      PROCESS_VAL(OpType::ADD, "+", "ADD ");
      PROCESS_VAL(OpType::INC, "++", "INC ");
      PROCESS_VAL(OpType::ADD_ASSIGN, "+=", "ADD_ASSIGN ");
      PROCESS_VAL(OpType::SUB, "-", "SUB ");
      PROCESS_VAL(OpType::DEC, "--", "DEC ");
      PROCESS_VAL(OpType::SUB_ASSIGN, "-=", "SUB_ASSIGN ");
      PROCESS_VAL(OpType::MUL_ASSIGN, "*=", "MUL_ASSIGN ");
      PROCESS_VAL(OpType::DIV, "/", "DIV ");
      PROCESS_VAL(OpType::DIV_ASSIGN, "/=", "DIV_ASSIGN ");
      PROCESS_VAL(OpType::MOD, "%", "MOD ");
      PROCESS_VAL(OpType::MOD_ASSIGN, "%=", "MOD_ASSIGN ");
      PROCESS_VAL(OpType::BITWISE_AND_ASSIGN, "&=", "BITWISE_AND_ASSIGN ");
      PROCESS_VAL(OpType::BITWISE_OR, "|", "BITWISE_OR ");
      PROCESS_VAL(OpType::BITWISE_OR_ASSIGN, "|=", "BITWISE_OR_ASSIGN ");
      PROCESS_VAL(OpType::BITWISE_XOR, "^", "BITWISE_XOR ");
      PROCESS_VAL(OpType::BITWISE_XOR_ASSIGN, "^=", "BITWISE_XOR_ASSIGN ");
      PROCESS_VAL(OpType::BITWISE_NOT, "~", "BITWISE_NOT ");
      PROCESS_VAL(OpType::AND, "&&", "AND ");
      PROCESS_VAL(OpType::AND_ASSIGN, "&&=", "AND_ASSIGN ");
      PROCESS_VAL(OpType::OR, "||", "OR ");
      PROCESS_VAL(OpType::OR_ASSIGN, "||=", "OR_ASSIGN ");
      PROCESS_VAL(OpType::NOT, "!", "NOT ");
      PROCESS_VAL(OpType::SHL, "<<", "SHL ");
      PROCESS_VAL(OpType::SHL_ASSIGN, "<<=", "SHL_ASSIGN ");
      PROCESS_VAL(OpType::SHR, ">>", "SHR ");
      PROCESS_VAL(OpType::SHR_ASSIGN, ">>=", "SHR_ASSIGN ");
      PROCESS_VAL(OpType::LESS, "<", "LESS ");
      PROCESS_VAL(OpType::LESS_EQUAL, "<=", "LESS_EQUAL ");
      PROCESS_VAL(OpType::EQUAL, "==", "EQUAL ");
      PROCESS_VAL(OpType::INEQUAL, "!=", "INEQUAL ");
      PROCESS_VAL(OpType::GREATER, ">", "GREATER ");
      PROCESS_VAL(OpType::GREATER_EQUAL, ">=", "GREATER_EQUAL ");
      PROCESS_VAL(OpType::CONCAT, "##", "CONCAT ");
      PROCESS_VAL(OpType::ASTERISK, "*", "ASTERISK ");
      PROCESS_VAL(OpType::AMPERSAND, "&", "AMPERSAND ");
      PROCESS_VAL(OpType::QUESTION, "?", "QUESTION ");
      PROCESS_VAL(OpType::COMMA, ",", "COMMA ");
      PROCESS_VAL(OpType::COLON, ":", "COLON ");
      PROCESS_VAL(OpType::SEMICOLON, ";", "SEMICOLON ");
      PROCESS_VAL(OpType::DOT, ".", "DOT ");
      PROCESS_VAL(OpType::ARROW, "->", "ARROW ");
      PROCESS_VAL(OpType::L_BRACE, "{", "L_BRACE ");
      PROCESS_VAL(OpType::R_BRACE, "}", "R_BRACE ");
      PROCESS_VAL(OpType::L_SQUARE, "[", "L_SQUARE ");
      PROCESS_VAL(OpType::R_SQUARE, "]", "R_SQUARE ");
      PROCESS_VAL(OpType::L_PAREN, "(", "L_PAREN ");
      PROCESS_VAL(OpType::R_PAREN, ")", "R_PAREN ");
    }
#undef PROCESS_VAL
    return formatter<string_view>::format(s, ctx);
  }
};

template <> struct fmt::formatter<ReservedWordType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(const ReservedWordType &p, FormatContext &ctx) {
    string_view s = "unknown";
#define PROCESS_VAL(p, name)                                                   \
  case (p):                                                                    \
    s = name;                                                                  \
    break;
    switch (p) {
      PROCESS_VAL(ReservedWordType::CHAR, "char");
      PROCESS_VAL(ReservedWordType::UNSIGNED, "unsigned");
      PROCESS_VAL(ReservedWordType::UNION, "union");
      PROCESS_VAL(ReservedWordType::INT, "int");
      PROCESS_VAL(ReservedWordType::SIGNER, "signer");
      PROCESS_VAL(ReservedWordType::TYPEDEF, "typedef");
      PROCESS_VAL(ReservedWordType::LONG, "long");
      PROCESS_VAL(ReservedWordType::CONST, "const");
      PROCESS_VAL(ReservedWordType::SIZEOF, "sizeof");
      PROCESS_VAL(ReservedWordType::FLOAT, "float");
      PROCESS_VAL(ReservedWordType::STATIC, "static");
      PROCESS_VAL(ReservedWordType::IF, "if");
      PROCESS_VAL(ReservedWordType::DOUBLE, "double");
      PROCESS_VAL(ReservedWordType::EXTERN, "extern");
      PROCESS_VAL(ReservedWordType::ELSE, "else");
      PROCESS_VAL(ReservedWordType::VOID, "void");
      PROCESS_VAL(ReservedWordType::STRUCT, "struct");
    }
#undef PROCESS_VAL
    return formatter<string_view>::format(s, ctx);
  }
};

template <> struct fmt::formatter<Token> {
  bool is_details = false;
  constexpr auto parse(format_parse_context &ctx) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'd')) {
      it++;
      is_details = true;
    }
    if (it != end && *it != '}')
      throw format_error("invalid format");
    return it;
  }

  template <typename FormatContext>
  auto format(const Token &t, FormatContext &ctx) {
    // return format_to(ctx.out(),
    //                  presentation == 'f' ? "({:.1f}, {:.1f})"
    //                                      : "({:.1e}, {:.1e})",
    //                  p.x, p.y);
    if (is_details) {
      if (t.is_op()) {
        return format_to(ctx.out(), "<OP>\t{:d}\tLoc=<{}:{}>", t.as_op(),
                         t.p_token.row, t.p_token.col);
      } else if (t.is_reserved_word()) {
        return format_to(ctx.out(), "<RESERVED>\t{}\tLoc=<{}:{}>",
                         t.as_reserved_word(), t.p_token.row, t.p_token.col);
      } else if (t.is_ident()) {
        return format_to(ctx.out(), "<IDENT>\t{}\tLoc=<{}:{}>", t.as_ident(),
                         t.p_token.row, t.p_token.col);
      } else if (t.is_number()) {
        return format_to(ctx.out(), "<NUMBER>\t{}\tLoc=<{}:{}>", t.as_number(),
                         t.p_token.row, t.p_token.col);
      } else if (t.is_string()) {
        return format_to(ctx.out(), "<STRING>\t{}\tLoc=<{}:{}>", t.as_string(),
                         t.p_token.row, t.p_token.col);
      } else {
        return format_to(ctx.out(), "<CHAR>\t{}\tLoc=<{}:{}>", t.as_char(),
                         t.p_token.row, t.p_token.col);
      }
    } else {
      if (t.is_op()) {
        return format_to(ctx.out(), "{}", t.as_op());
      } else if (t.is_reserved_word()) {
        return format_to(ctx.out(), "{}", t.as_reserved_word());
      } else if (t.is_ident()) {
        return format_to(ctx.out(), "<id>");
      } else if (t.is_number()) {
        return format_to(ctx.out(), "<num>");
      } else if (t.is_string()) {
        return format_to(ctx.out(), "<str>");
      } else if (t.is_char()) {
        return format_to(ctx.out(), "<char>");
      } else {
        throw "invalid token";
      }
    }
  }
};
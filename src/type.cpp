#include "type.h"
#include <string>

Token::Token(OpType op_type, Position pos)
    : token_type(TokenType::OP), p_token(pos) {
  token_value.op_type = op_type;
}

Token::Token(ReservedWordType reserved_word, Position pos)
    : token_type(TokenType::ReservedWord), p_token(pos) {

  token_value.reserved_word = reserved_word;
}

Token::Token(char *data, bool is_number, Position pos)
    : token_type(is_number ? TokenType::Number : TokenType::Ident),
      p_token(pos) {

  token_value.data = data;
}

bool Token::is_op() const { return this->token_type == TokenType::OP; }

bool Token::is_reserved_word() const {
  return this->token_type == TokenType::ReservedWord;
}

bool Token::is_ident() const { return this->token_type == TokenType::Ident; }

bool Token::is_number() const { return this->token_type == TokenType::Number; }

OpType Token::as_op() const {
  if (!this->is_op()) {
    throw "the token is not op";
  }
  return this->token_value.op_type;
}

ReservedWordType Token::as_reserved_word() const {
  if (!this->is_reserved_word()) {
    throw "the token is not reserved_word";
  }
  return this->token_value.reserved_word;
}

char *Token::as_ident() const {
  if (!this->is_ident()) {
    throw "the token is not ident";
  }
  return this->token_value.data;
}

char *Token::as_number() const {
  if (!this->is_number()) {
    throw "the token is not number";
  }
  return this->token_value.data;
}

std::ostream &operator<<(std::ostream &out, const OpType value) {
  const char *s = 0;
#define PROCESS_VAL(p, name)                                                   \
  case (p):                                                                    \
    s = name;                                                                  \
    break;
  switch (value) {
    PROCESS_VAL(OpType::ASSIGN, "ASSIGN '='");
    PROCESS_VAL(OpType::ADD, "ADD '+'");
    PROCESS_VAL(OpType::INC, "INC '++'");
    PROCESS_VAL(OpType::ADD_ASSIGN, "ADD_ASSIGN '+='");
    PROCESS_VAL(OpType::SUB, "SUB '-'");
    PROCESS_VAL(OpType::DEC, "DEC '--'");
    PROCESS_VAL(OpType::SUB_ASSIGN, "SUB_ASSIGN '-='");
    PROCESS_VAL(OpType::MUL_ASSIGN, "MUL_ASSIGN '*='");
    PROCESS_VAL(OpType::DIV, "DIV '/'");
    PROCESS_VAL(OpType::DIV_ASSIGN, "DIV_ASSIGN '/='");
    PROCESS_VAL(OpType::MOD, "MOD '%'");
    PROCESS_VAL(OpType::MOD_ASSIGN, "MOD_ASSIGN '%='");
    PROCESS_VAL(OpType::BITWISE_AND_ASSIGN, "BITWISE_AND_ASSIGN '&='");
    PROCESS_VAL(OpType::BITWISE_OR, "BITWISE_OR '|'");
    PROCESS_VAL(OpType::BITWISE_OR_ASSIGN, "BITWISE_OR_ASSIGN '|='");
    PROCESS_VAL(OpType::BITWISE_XOR, "BITWISE_XOR '^'");
    PROCESS_VAL(OpType::BITWISE_XOR_ASSIGN, "BITWISE_XOR_ASSIGN '^='");
    PROCESS_VAL(OpType::BITWISE_NOT, "BITWISE_NOT '~'");
    PROCESS_VAL(OpType::AND, "AND '&&'");
    PROCESS_VAL(OpType::AND_ASSIGN, "AND_ASSIGN '&&='");
    PROCESS_VAL(OpType::OR, "OR '||'");
    PROCESS_VAL(OpType::OR_ASSIGN, "OR_ASSIGN '||='");
    PROCESS_VAL(OpType::NOT, "NOT '!'");
    PROCESS_VAL(OpType::SHL, "SHL '<<'");
    PROCESS_VAL(OpType::SHL_ASSIGN, "SHL_ASSIGN '<<='");
    PROCESS_VAL(OpType::SHR, "SHR '>>'");
    PROCESS_VAL(OpType::SHR_ASSIGN, "SHR_ASSIGN '>>='");
    PROCESS_VAL(OpType::LESS, "LESS '<'");
    PROCESS_VAL(OpType::LESS_EQUAL, "LESS_EQUAL '<='");
    PROCESS_VAL(OpType::EQUAL, "EQUAL '=='");
    PROCESS_VAL(OpType::INEQUAL, "INEQUAL '!='");
    PROCESS_VAL(OpType::GREATER, "GREATER '>'");
    PROCESS_VAL(OpType::GREATER_EQUAL, "GREATER_EQUAL '>='");
    PROCESS_VAL(OpType::CONCAT, "CONCAT '##'");
    PROCESS_VAL(OpType::ASTERISK, "ASTERISK '*'");
    PROCESS_VAL(OpType::AMPERSAND, "AMPERSAND '&'");
    PROCESS_VAL(OpType::QUESTION, "QUESTION '?'");
    PROCESS_VAL(OpType::COMMA, "COMMA ','");
    PROCESS_VAL(OpType::COLON, "COLON ':'");
    PROCESS_VAL(OpType::SEMICOLON, "SEMICOLON ';'");
    PROCESS_VAL(OpType::DOT, "DOT '.'");
    PROCESS_VAL(OpType::ARROW, "ARROW '->'");
    PROCESS_VAL(OpType::L_BRACE, "L_BRACE '{'");
    PROCESS_VAL(OpType::R_BRACE, "R_BRACE '}'");
    PROCESS_VAL(OpType::L_SQUARE, "L_SQUARE '['");
    PROCESS_VAL(OpType::R_SQUARE, "R_SQUARE ']'");
    PROCESS_VAL(OpType::L_PAREN, "L_PAREN '('");
    PROCESS_VAL(OpType::R_PAREN, "R_PAREN ')'");
  }
#undef PROCESS_VAL
  return out << s;
}

std::ostream &operator<<(std::ostream &out, const ReservedWordType value) {
  const char *s = 0;
#define PROCESS_VAL(p, name)                                                   \
  case (p):                                                                    \
    s = name;                                                                  \
    break;
  switch (value) {
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

  return out << s;
}

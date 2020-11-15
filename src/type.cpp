#include "type.h"
#include <string>

Token::Token(OpType op_type, Position pos)
    : p_token(pos), token_type(TokenType::OP)
{
  token_value.op_type = op_type;
}

Token::Token(ReservedWordType reserved_word, Position pos)
    : p_token(pos), token_type(TokenType::ReservedWord)
{

  token_value.reserved_word = reserved_word;
}

Token::Token(Token::TokenType type, char *data, Position pos)
    : p_token(pos), token_type(type)
{
  token_value.data = data;
}

bool Token::is_op() const { return this->token_type == TokenType::OP; }

bool Token::is_reserved_word() const
{
  return this->token_type == TokenType::ReservedWord;
}

bool Token::is_ident() const { return this->token_type == TokenType::Ident; }

bool Token::is_number() const { return this->token_type == TokenType::Number; }

bool Token::is_string() const { return this->token_type == TokenType::String; }

bool Token::is_char() const { return this->token_type == TokenType::Char; }

bool Token::is_null() const { return this->token_type == TokenType::Null; }

OpType Token::as_op() const
{
  if (!this->is_op())
  {
    throw "the token is not op";
  }
  return this->token_value.op_type;
}

ReservedWordType Token::as_reserved_word() const
{
  if (!this->is_reserved_word())
  {
    throw "the token is not reserved_word";
  }
  return this->token_value.reserved_word;
}

char *Token::as_ident() const
{
  if (!this->is_ident())
  {
    throw "the token is not ident";
  }
  return this->token_value.data;
}

char *Token::as_number() const
{
  if (!this->is_number())
  {
    throw "the token is not number";
  }
  return this->token_value.data;
}

char *Token::as_string() const
{
  if (!this->is_string())
  {
    throw "the token is not string";
  }
  return this->token_value.data;
}

char *Token::as_char() const
{
  if (!this->is_char())
  {
    throw "the token is not char";
  }
  return this->token_value.data;
}

namespace plog
{
  Record &operator<<(Record &record, const OpType &o)
  {
    const char *s = 0;
#define PROCESS_VAL(p, name) \
  case (p):                  \
    s = name;                \
    break;
    switch (o)
    {
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
    return record << s;
  }

  Record &operator<<(Record &record, const ReservedWordType &r)
  {
    const char *s = 0;
#define PROCESS_VAL(p, name) \
  case (p):                  \
    s = name;                \
    break;
    switch (r)
    {
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

    return record << s;
  }

  Record &operator<<(Record &record, const Token &t)
  {
    if (t.is_op())
    {
      record << "<OP>\t" << t.as_op();
    }
    else if (t.is_reserved_word())
    {
      record << "<RESERVED>\t" << t.as_reserved_word();
    }
    else if (t.is_ident())
    {
      record << "<IDNET>\t" << t.as_ident();
    }
    else if (t.is_number())
    {
      record << "<NUMBER>\t" << t.as_number();
    }
    else if (t.is_string())
    {
      record << "<STRING>\t" << t.as_string();
    }
    else if (t.is_char())
    {
      record << "<CHAR>\t" << t.as_char();
    }
    record << "\tLoc=<" << t.p_token.row << ":" << t.p_token.col << ">";

    return record;
  }
} // namespace plog

Token::TokenType Token::type() const { return this->token_type; }
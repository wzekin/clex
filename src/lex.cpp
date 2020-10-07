#include "lex.h"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <regex>
#include <string>

Lex::Lex(const char *path) : reader(new Reader(path)) {}

static inline bool is_ident_byte(const char c) {
  return std::isalnum(c) || c == '_';
}

void Lex::parse_ident() {
  size_t index = 0;
  char *token = (char *)malloc(sizeof(char) * 50);
  memset(token, 0, 50);
  token[index++] = this->reader->peek();
  while (is_ident_byte(this->reader->front_peek())) {
    token[index++] = this->reader->front_peek();
    this->reader->front_ahead();
  }
  auto res = reserved_word.find(token);
  if (res != reserved_word.end()) {
    this->tokens.push_back(Token(res.value(), this->reader->pos()));
  } else {
    this->tokens.push_back(Token(token, false, this->reader->pos()));
  }
  this->reader->ahead();
}

void Lex::parse_number() {
  size_t index = 0;
  char *token = (char *)malloc(sizeof(char) * 50);
  memset(token, 0, 50);
  token[index++] = this->reader->peek();
  while (is_ident_byte(this->reader->front_peek())) {
    token[index++] = this->reader->front_peek();
    this->reader->front_ahead();
  }

  //     "^(\-|\+)?"
  //     "("
  //     "0|"
  //     "((\d*.\d+)|(\d+.\d*))(e(+|-)?\d+)?|"
  //     "[1-9]\d*|"
  //     "0[1-7][0-7]*|"
  //     "0x[1-9a-f][0-9a-f]*|"
  //     "0x([1-9a-f][0-9a-f]*.[0-9a-f]*|[0-9a-f]*.[1-9a-f][0-9a-f]*)p(+|-)?[0-9a-f]+"
  //     ")"
  //     "(ul|lu|l|u)?$"

  // std::regex
  // re("^(\-|\+)?(0|((\d*.\d+)|(\d+.\d*))(e(\+|\-)?\d+)?|[1-9]\d*|0[1-"
  //               "7][0-7]*|0x[1-9a-f][0-9a-f]*|0x([1-9a-f][0-9a-f]*.[0-9a-f]*|["
  //               "0-9a-f]*.[1-9a-f][0-9a-f]*)p(\+|\-)?[0-9a-f]+)(ul|lu|l|u)?$");
  this->tokens.push_back(Token(token, true, this->reader->pos()));
  this->reader->ahead();
  return;
}

void Lex::parse_macro_or_line_comment() {
  while (this->reader->front_peek() != '\n') {
    this->reader->front_ahead();
  }
  this->reader->ahead();
}

void Lex::parse_block_comment() {
  int stat = 0;
  while (stat != 2) {
    if (stat == 0 && this->reader->front_peek() == '*') {
      stat = 1;
    } else if (stat == 1 && this->reader->front_peek() == '/') {
      stat = 2;
    } else if (stat == 1) {
      stat = 0;
    }
    this->reader->front_ahead();
  }
  this->reader->ahead();
}

void Lex::parse() {
  while (true) {
    switch (this->reader->peek()) {
    case '+': {
      if (this->reader->front_peek() == '+') {
        this->tokens.push_back(Token(OpType::INC, this->reader->pos()));
        this->reader->front_ahead();
      } else if (this->reader->front_peek() == '=') {
        this->tokens.push_back(Token(OpType::ADD_ASSIGN, this->reader->pos()));
        this->reader->front_ahead();
      } else {
        this->tokens.push_back(Token(OpType::ADD, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '=': {
      if (this->reader->front_peek() == '=') {
        this->tokens.push_back(Token(OpType::EQUAL, this->reader->pos()));
        this->reader->front_ahead();
      } else {
        this->tokens.push_back(Token(OpType::ASSIGN, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '-': {
      if (this->reader->front_peek() == '-') {
        this->tokens.push_back(Token(OpType::DEC, this->reader->pos()));
        this->reader->front_ahead();
      } else if (this->reader->front_peek() == '>') {
        this->tokens.push_back(Token(OpType::ARROW, this->reader->pos()));
        this->reader->front_ahead();
      } else if (this->reader->front_peek() == '=') {
        this->tokens.push_back(Token(OpType::SUB_ASSIGN, this->reader->pos()));
        this->reader->front_ahead();
      } else {
        this->tokens.push_back(Token(OpType::SUB, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '*': {
      if (this->reader->front_peek() == '=') {
        this->tokens.push_back(Token(OpType::MUL_ASSIGN, this->reader->pos()));
        this->reader->front_ahead();
      } else {
        this->tokens.push_back(Token(OpType::ASTERISK, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '/': {
      if (this->reader->front_peek() == '/') {
        this->parse_macro_or_line_comment();
      } else if (this->reader->front_peek() == '*') {
        this->parse_block_comment();
      } else if (this->reader->front_peek() == '=') {
        this->tokens.push_back(Token(OpType::DIV_ASSIGN, this->reader->pos()));
        this->reader->front_ahead();
        this->reader->ahead();
      } else {
        this->tokens.push_back(Token(OpType::DIV, this->reader->pos()));
        this->reader->ahead();
      }
      break;
    }
    case '%': {
      if (this->reader->front_peek() == '=') {
        this->tokens.push_back(Token(OpType::MOD_ASSIGN, this->reader->pos()));
        this->reader->front_ahead();
      } else {
        this->tokens.push_back(Token(OpType::MOD, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '&': {
      if (this->reader->front_peek() == '=') {
        this->tokens.push_back(
            Token(OpType::BITWISE_AND_ASSIGN, this->reader->pos()));
        this->reader->front_ahead();
      } else if (this->reader->front_peek() == '&') {
        this->reader->front_ahead();
        if (this->reader->front_peek() == '=') {
          this->reader->front_ahead();
          this->tokens.push_back(
              Token(OpType::AND_ASSIGN, this->reader->pos()));
        } else {
          this->tokens.push_back(Token(OpType::AND, this->reader->pos()));
        }
      } else {
        this->tokens.push_back(Token(OpType::AMPERSAND, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '|': {
      if (this->reader->front_peek() == '=') {
        this->tokens.push_back(
            Token(OpType::BITWISE_OR_ASSIGN, this->reader->pos()));
        this->reader->front_ahead();
      } else if (this->reader->front_peek() == '|') {
        this->reader->front_ahead();
        if (this->reader->front_peek() == '=') {
          this->reader->front_ahead();
          this->tokens.push_back(Token(OpType::OR_ASSIGN, this->reader->pos()));
        } else {
          this->tokens.push_back(Token(OpType::OR, this->reader->pos()));
        }
      } else {
        this->tokens.push_back(Token(OpType::BITWISE_OR, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '^': {
      if (this->reader->front_peek() == '=') {
        this->tokens.push_back(
            Token(OpType::BITWISE_XOR_ASSIGN, this->reader->pos()));
        this->reader->front_ahead();
      } else {
        this->tokens.push_back(Token(OpType::BITWISE_XOR, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '~': {
      this->tokens.push_back(Token(OpType::BITWISE_NOT, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case '!': {
      if (this->reader->front_peek() == '=') {
        this->tokens.push_back(Token(OpType::INEQUAL, this->reader->pos()));
        this->reader->front_ahead();
      } else {
        this->tokens.push_back(Token(OpType::NOT, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '<': {
      if (this->reader->front_peek() == '=') {
        this->tokens.push_back(Token(OpType::LESS_EQUAL, this->reader->pos()));
        this->reader->front_ahead();
      } else if (this->reader->front_peek() == '<') {
        this->reader->front_ahead();
        if (this->reader->front_peek() == '=') {
          this->reader->front_ahead();
          this->tokens.push_back(
              Token(OpType::SHL_ASSIGN, this->reader->pos()));
        } else {
          this->tokens.push_back(Token(OpType::SHL, this->reader->pos()));
        }
      } else {
        this->tokens.push_back(Token(OpType::LESS, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '>': {
      if (this->reader->front_peek() == '=') {
        this->tokens.push_back(
            Token(OpType::GREATER_EQUAL, this->reader->pos()));
        this->reader->front_ahead();
      } else if (this->reader->front_peek() == '<') {
        this->reader->front_ahead();
        if (this->reader->front_peek() == '=') {
          this->reader->front_ahead();
          this->tokens.push_back(
              Token(OpType::SHR_ASSIGN, this->reader->pos()));
        } else {
          this->tokens.push_back(Token(OpType::SHR, this->reader->pos()));
        }
      } else {
        this->tokens.push_back(Token(OpType::GREATER, this->reader->pos()));
      }
      this->reader->ahead();
      break;
    }
    case '#': {
      this->parse_macro_or_line_comment();
      break;
    }
    case '?': {
      this->tokens.push_back(Token(OpType::QUESTION, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case ',': {
      this->tokens.push_back(Token(OpType::COMMA, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case ':': {
      this->tokens.push_back(Token(OpType::COLON, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case ';': {
      this->tokens.push_back(Token(OpType::SEMICOLON, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case '.': {
      this->tokens.push_back(Token(OpType::DOT, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case '{': {
      this->tokens.push_back(Token(OpType::L_BRACE, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case '}': {
      this->tokens.push_back(Token(OpType::R_BRACE, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case '[': {
      this->tokens.push_back(Token(OpType::L_SQUARE, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case ']': {
      this->tokens.push_back(Token(OpType::R_SQUARE, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case '(': {
      this->tokens.push_back(Token(OpType::L_PAREN, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    case ')': {
      this->tokens.push_back(Token(OpType::R_PAREN, this->reader->pos()));
      this->reader->ahead();
      break;
    }
    default: {
      if (std::isdigit(this->reader->peek())) {
        this->parse_number();
      } else if (is_ident_byte(this->reader->peek())) {
        this->parse_ident();
      } else {
        this->reader->ahead();
        if (this->reader->peek() == '\0' && this->reader->is_eof()) {
          return;
        }
        continue;
      }
      if (this->reader->peek() == '\0' && this->reader->is_eof()) {
        return;
      }
    }
    }
  }
}

void Lex::print() {
  std::cout << this->tokens.size() << std::endl;
  for (auto a : this->tokens) {
    std::cout << a << std::endl;
  }
}
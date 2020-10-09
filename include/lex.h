#pragma once
#include "reader.h"
#include "type.h"
#include <memory>
#include <tsl/htrie_map.h>
#include <vector>

const tsl::htrie_map<char, ReservedWordType>
    reserved_word({{"char", ReservedWordType::CHAR},
                   {"unsigned", ReservedWordType::UNSIGNED},
                   {"union", ReservedWordType::UNION},
                   {"int", ReservedWordType::INT},
                   {"signer", ReservedWordType::SIGNER},
                   {"typedef", ReservedWordType::TYPEDEF},
                   {"long", ReservedWordType::LONG},
                   {"const", ReservedWordType::CONST},
                   {"sizeof", ReservedWordType::SIZEOF},
                   {"float", ReservedWordType::FLOAT},
                   {"static", ReservedWordType::STATIC},
                   {"if", ReservedWordType::IF},
                   {"double", ReservedWordType::DOUBLE},
                   {"extern", ReservedWordType::EXTERN},
                   {"else", ReservedWordType::ELSE},
                   {"void", ReservedWordType::VOID},
                   {"struct", ReservedWordType::STRUCT}});

class Lex {
public:
  Lex(const char *path);

  void parse();

  void print();

private:
  std::unique_ptr<Reader> reader;

  std::vector<Token> tokens;

  void parse_ident();

  void parse_number();

  void parse_string();

  void parse_char();

  void parse_macro_or_line_comment();

  void parse_block_comment();
};

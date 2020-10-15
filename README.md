# C语言词法分析程序的设计与实现

> 2018211236 王泽坤

## 概述

### 实验内容及要求:

1. 可以识别出用C语言编写的源程序中的每个单词符号,并以记号的形式输出每个单词符号。
2. 可以识别并跳过源程序中的注释。
3. 可以统计源程序中的语句行数、各类单词的个数、以及字符总数,并输出统计结果。
4. 检查源程序中存在的词法错误,并报告错误所在的位置。
5. 对源程序中出现的错误进行适当的恢复,使词法分析可以继续进行,对源程序进行一次扫描,即可检查并报告源程序中存在的所有词法错误。

### 实验环境

* 操作系统：Linux
* 编程语言：C++

## 程序设计说明

### 模块划分

#### Reader

该类是对文件读取的进一步抽象化，用来记录总字符数，记录字符的位置，内部是使用ifstream以及一个buffer，类的实现如下：

``` c++
class Reader {
public:
  /**
     Reader 构造函数
   */
  Reader(const char *path);

  /**
     将当前指针前移一位，并把前向指针变为当前指针的前一位
   */
  void ahead();

  /**
     将前向指针前移一位
   */
  void front_ahead();

  /**
     返回当前的地址
   */
  char peek() const;

  /**
     返回前向指针的地址
   */
  char front_peek() const;

  /**
     是否读到结尾
   */
  bool is_eof() const;

  /**
   * 返回当前位置
   */
  Position pos() const;

  /**
   * 字符总数
   */
  size_t count() const;

private:
  std::ifstream file;
  char buffer[READER_BUFFER * 2];
  size_t index;
  size_t front_index;

  Position p_index;

  Position p_front_index;

  void read_buffer(char *buffer);

  size_t count_;
};
```

#### Type

定义了Token类，用于记录Token数据以及格式化Token的输出，类的实现如下

```c++
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
  };

  Token(OpType op_type, Position pos);
  Token(ReservedWordType reserved_word, Position pos);
  Token(char *data, Token::TokenType type, Position pos);

  bool is_op() const;
  bool is_reserved_word() const;
  bool is_ident() const;
  bool is_number() const;
  bool is_string() const;
  bool is_char() const;

  OpType as_op() const;
  ReservedWordType as_reserved_word() const;
  char *as_ident() const;
  char *as_number() const;
  char *as_string() const;
  char *as_char() const;

  TokenType type() const;

  Position p_token;

private:
  union TokenValue {
    OpType op_type;
    ReservedWordType reserved_word;
    char *data;
  };

  TokenType token_type;

  TokenValue token_value;
};
```

* `enum class OpType`：标记所有的操作符
* `enum class ReservedWordType`：标记所有的保留字
* `enum class TokenType`：把所有的字符分为操作符、保留字、标识符、数字、字符串、字符六类，并使用Union存储数据3

#### Lex

词法分析器的主体部分，在这个类内完成Token的生成与统计，类的定义如下：

```c++
class Lex {
public:
  Lex(const char *path);

  // 解析并输出数据
  void parse();

  // 统计并综合数据
  void report();

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
```

## 测试程序

### 完全正确的程序

#### 源代码

``` c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dns_port.h"
#include "hashmap.h"
#include "log.h"
#include "utils.h"

struct Hashmap* Hashmap_create(size_t size,
                               size_t (*hashcode)(const void* key),
                               bool (*eq)(const void* self, const void* other),
                               void (*key_free)(void* key),
                               void (*data_free)(void* data)) {
  size = hight_bit(size);
  struct Hashmap* map = malloc(sizeof(struct Hashmap));
  map->data = calloc(sizeof(struct DataItem), size);
  map->bucket_size = size;
  map->hashcode = hashcode;
  map->size = 0;
  map->eq = eq;
  map->key_free = key_free;
  map->data_free = data_free;
  return map;
}

struct DataItem* Hashmap_search(const struct Hashmap* self, const void* key) {
  size_t hashIndex = self->hashcode(key) & (self->bucket_size - 1);
  struct DataItem* item = self->data[hashIndex];
  while (item) {
    if (self->eq(item->key, key))
      return item;
    item = item->hash_next;
  }

  return NULL;
}

void Hashmap_insert(struct Hashmap* self, void* key, void* data) {
  struct DataItem* item = malloc(sizeof(struct DataItem));
  item->key = key;
  item->data = data;
  item->hash_next = NULL;
  int hashIndex = self->hashcode(key) & (self->bucket_size - 1);
  struct DataItem* temp = self->data[hashIndex];
  if (!temp) {
    self->data[hashIndex] = item;
  } else {
    while (temp->hash_next) {
      temp = temp->hash_next;
    }
    temp->hash_next = item;
  }

  self->size++;
}

struct DataItem* Hashmap_delete(struct Hashmap* self, void* key) {
  int hashIndex = self->hashcode(key) & (self->bucket_size - 1);
  struct DataItem* item = self->data[hashIndex];
  if (self->eq(item->key, key)) {
    self->data[hashIndex] = item->hash_next;
    self->size--;
    return item;
  }
  while (item->hash_next) {
    if (self->eq(item->hash_next->key, key)) {
      struct DataItem* temp = item->hash_next;
      item->hash_next = temp->hash_next;
      self->size--;
      return temp;
    }
    item = item->hash_next;
  }
  return NULL;
}

size_t Hashmap_size(struct Hashmap* self) {
  return self->size;
}

void Hashmap_free(struct Hashmap* map) {
  struct DataItem *item, *temp;
  for (int i = 0; i <= map->bucket_size; i++) {
    item = map->data[i];
    while (item) {
      temp = item;
      item = item->hash_next;
      if (temp->key) {
        map->key_free(temp->key);
      }
      if (temp->data) {
        map->data_free(temp->data);
      }
      free(temp);
    }
  }
  free(map->data);
  free(map);
}
```

#### 输出

``` shell
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<11:1>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap Loc=<11:8>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<11:15>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap_create  Loc=<11:17>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<11:31>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  size_t  Loc=<11:32>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<11:39>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<11:43>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  size_t  Loc=<12:32>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<12:39>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<12:40>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  hashcode        Loc=<12:41>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<12:49>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<12:50>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       const   Loc=<12:51>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<12:57>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<12:61>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<12:63>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<12:66>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<12:67>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  bool    Loc=<13:32>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<13:37>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<13:38>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  eq      Loc=<13:39>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<13:41>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<13:42>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       const   Loc=<13:43>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<13:49>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<13:53>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<13:55>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<13:59>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       const   Loc=<13:61>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<13:67>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<13:71>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  other   Loc=<13:73>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<13:78>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<13:79>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<14:32>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<14:37>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<14:38>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  key_free        Loc=<14:39>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<14:47>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<14:48>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<14:49>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<14:53>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<14:55>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<14:58>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<14:59>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<15:32>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<15:37>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<15:38>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  data_free       Loc=<15:39>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<15:48>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<15:49>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<15:50>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<15:54>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<15:56>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<15:60>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<15:61>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<15:63>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<16:3>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<16:8>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  hight_bit       Loc=<16:10>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<16:19>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<16:20>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<16:24>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<16:25>
2020-10-14 10:59:10.938 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<17:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap Loc=<17:10>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<17:17>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<17:19>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<17:23>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  malloc  Loc=<17:25>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<17:31>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       sizeof  Loc=<17:32>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<17:38>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<17:39>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap Loc=<17:46>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<17:53>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<17:54>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<17:55>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<18:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<18:6>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<18:8>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<18:13>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  calloc  Loc=<18:15>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<18:21>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       sizeof  Loc=<18:22>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<18:28>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<18:29>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<18:36>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<18:44>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<18:45>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<18:47>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<18:51>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<18:52>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<19:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<19:6>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  bucket_size     Loc=<19:8>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<19:20>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<19:22>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<19:26>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<20:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<20:6>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  hashcode        Loc=<20:8>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<20:17>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  hashcode        Loc=<20:19>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<20:27>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<21:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<21:6>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<21:8>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<21:13>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <NUMBER> 0       Loc=<21:15>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<21:16>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<22:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<22:6>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  eq      Loc=<22:8>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<22:11>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  eq      Loc=<22:13>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<22:15>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<23:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<23:6>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  key_free        Loc=<23:8>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<23:17>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  key_free        Loc=<23:19>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<23:27>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<24:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<24:6>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  data_free       Loc=<24:8>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<24:18>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  data_free       Loc=<24:20>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<24:29>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  return  Loc=<25:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<25:10>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<25:13>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<26:1>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<28:1>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<28:8>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<28:16>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap_search  Loc=<28:18>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<28:32>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       const   Loc=<28:33>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<28:39>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap Loc=<28:46>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<28:53>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<28:55>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<28:59>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       const   Loc=<28:61>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<28:67>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<28:71>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<28:73>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<28:76>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<28:78>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  size_t  Loc=<29:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  hashIndex       Loc=<29:10>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<29:20>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<29:22>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<29:26>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  hashcode        Loc=<29:28>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<29:36>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<29:37>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<29:40>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     AMPERSAND '&'   Loc=<29:42>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<29:44>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<29:45>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<29:49>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  bucket_size     Loc=<29:51>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SUB '-' Loc=<29:63>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <NUMBER> 1       Loc=<29:65>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<29:66>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<29:67>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<30:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<30:10>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<30:18>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<30:20>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<30:25>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<30:27>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<30:31>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<30:33>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_SQUARE '['    Loc=<30:37>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  hashIndex       Loc=<30:38>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_SQUARE ']'    Loc=<30:47>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<30:48>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  while   Loc=<31:3>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<31:9>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<31:10>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<31:14>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<31:16>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <RESERVED>       if      Loc=<32:5>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<32:8>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<32:9>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<32:13>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  eq      Loc=<32:15>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<32:17>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<32:18>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<32:22>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<32:24>
2020-10-14 10:59:10.939 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<32:27>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<32:29>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<32:32>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<32:33>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  return  Loc=<33:7>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<33:14>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<33:18>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<34:5>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<34:10>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<34:12>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<34:16>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<34:18>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<34:27>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<35:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  return  Loc=<37:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  NULL    Loc=<37:10>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<37:14>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<38:1>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<40:1>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap_insert  Loc=<40:6>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<40:20>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<40:21>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap Loc=<40:28>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<40:35>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<40:37>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<40:41>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<40:43>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<40:47>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<40:49>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<40:52>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<40:54>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<40:58>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<40:60>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<40:64>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<40:66>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<41:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<41:10>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<41:18>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<41:20>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<41:25>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  malloc  Loc=<41:27>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<41:33>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       sizeof  Loc=<41:34>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<41:40>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<41:41>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<41:48>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<41:56>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<41:57>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<41:58>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<42:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<42:7>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<42:9>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<42:13>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<42:15>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<42:18>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<43:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<43:7>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<43:9>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<43:14>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<43:16>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<43:20>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<44:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<44:7>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<44:9>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<44:19>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  NULL    Loc=<44:21>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<44:25>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       int     Loc=<45:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  hashIndex       Loc=<45:7>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<45:17>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<45:19>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<45:23>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  hashcode        Loc=<45:25>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<45:33>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<45:34>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<45:37>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     AMPERSAND '&'   Loc=<45:39>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<45:41>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<45:42>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<45:46>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  bucket_size     Loc=<45:48>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SUB '-' Loc=<45:60>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <NUMBER> 1       Loc=<45:62>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<45:63>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<45:64>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<46:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<46:10>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<46:18>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<46:20>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<46:25>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<46:27>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<46:31>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<46:33>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_SQUARE '['    Loc=<46:37>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  hashIndex       Loc=<46:38>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_SQUARE ']'    Loc=<46:47>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<46:48>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       if      Loc=<47:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<47:6>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     NOT '!' Loc=<47:7>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<47:8>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<47:12>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<47:14>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<48:5>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<48:9>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<48:11>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_SQUARE '['    Loc=<48:15>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  hashIndex       Loc=<48:16>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_SQUARE ']'    Loc=<48:25>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<48:27>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<48:29>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<48:33>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<49:3>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <RESERVED>       else    Loc=<49:5>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<49:10>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  while   Loc=<50:5>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<50:11>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<50:12>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<50:16>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<50:18>
2020-10-14 10:59:10.940 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<50:27>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<50:29>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<51:7>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<51:12>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<51:14>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<51:18>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<51:20>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<51:29>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<52:5>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<53:5>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<53:9>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<53:11>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<53:21>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<53:23>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<53:27>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<54:3>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<56:3>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<56:7>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<56:9>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     INC '++'        Loc=<56:13>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<56:15>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<57:1>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<59:1>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<59:8>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<59:16>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap_delete  Loc=<59:18>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<59:32>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<59:33>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap Loc=<59:40>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<59:47>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<59:49>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<59:53>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<59:55>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<59:59>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<59:61>
2020-10-14 10:59:10.941 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<59:64>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<59:66>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <RESERVED>       int     Loc=<60:3>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  hashIndex       Loc=<60:7>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<60:17>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<60:19>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<60:23>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  hashcode        Loc=<60:25>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<60:33>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<60:34>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<60:37>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     AMPERSAND '&'   Loc=<60:39>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<60:41>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<60:42>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<60:46>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  bucket_size     Loc=<60:48>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     SUB '-' Loc=<60:60>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <NUMBER> 1       Loc=<60:62>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<60:63>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<60:64>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<61:3>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<61:10>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<61:18>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<61:20>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<61:25>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<61:27>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<61:31>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<61:33>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     L_SQUARE '['    Loc=<61:37>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  hashIndex       Loc=<61:38>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     R_SQUARE ']'    Loc=<61:47>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<61:48>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <RESERVED>       if      Loc=<62:3>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<62:6>
2020-10-14 10:59:10.951 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<62:7>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<62:11>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <IDNET>  eq      Loc=<62:13>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<62:15>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<62:16>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<62:20>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<62:22>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<62:25>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<62:27>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<62:30>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<62:31>
2020-10-14 10:59:10.952 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<62:33>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<63:5>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<63:9>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<63:11>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     L_SQUARE '['    Loc=<63:15>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  hashIndex       Loc=<63:16>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     R_SQUARE ']'    Loc=<63:25>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<63:27>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<63:29>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<63:33>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<63:35>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<63:44>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<64:5>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<64:9>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<64:11>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     DEC '--'        Loc=<64:15>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<64:17>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  return  Loc=<65:5>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<65:12>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<65:16>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<66:3>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  while   Loc=<67:3>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<67:9>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<67:10>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<67:14>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<67:16>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<67:25>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<67:27>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <RESERVED>       if      Loc=<68:5>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<68:8>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<68:9>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<68:13>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  eq      Loc=<68:15>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<68:17>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<68:18>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<68:22>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<68:24>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<68:33>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<68:35>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<68:38>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<68:40>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<68:43>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<68:44>
2020-10-14 10:59:10.976 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<68:46>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<69:7>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<69:14>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<69:22>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<69:24>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<69:29>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<69:31>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<69:35>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<69:37>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<69:46>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<70:7>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<70:11>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<70:13>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<70:23>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<70:25>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<70:29>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<70:31>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<70:40>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<71:7>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<71:11>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<71:13>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     DEC '--'        Loc=<71:17>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<71:19>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  return  Loc=<72:7>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<72:14>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<72:18>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<73:5>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<74:5>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<74:10>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<74:12>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<74:16>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<74:18>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<74:27>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<75:3>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  return  Loc=<76:3>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  NULL    Loc=<76:10>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<76:14>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<77:1>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  size_t  Loc=<79:1>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap_size    Loc=<79:8>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<79:20>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<79:21>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap Loc=<79:28>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<79:35>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<79:37>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<79:41>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<79:43>
2020-10-14 10:59:10.977 INFO  [8231] [Lex::report@425] <IDNET>  return  Loc=<80:3>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  self    Loc=<80:10>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<80:14>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  size    Loc=<80:16>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<80:20>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<81:1>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <RESERVED>       void    Loc=<83:1>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap_free    Loc=<83:6>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<83:18>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<83:19>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  Hashmap Loc=<83:26>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<83:33>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<83:35>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<83:38>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<83:40>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <RESERVED>       struct  Loc=<84:3>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  DataItem        Loc=<84:10>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<84:19>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<84:20>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     COMMA ','       Loc=<84:24>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     ASTERISK '*'    Loc=<84:26>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<84:27>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<84:31>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  for     Loc=<85:3>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<85:7>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <RESERVED>       int     Loc=<85:8>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  i       Loc=<85:12>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<85:14>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <NUMBER> 0       Loc=<85:16>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<85:17>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  i       Loc=<85:19>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     LESS_EQUAL '<=' Loc=<85:21>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<85:24>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<85:27>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  bucket_size     Loc=<85:29>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<85:40>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <IDNET>  i       Loc=<85:42>
2020-10-14 10:59:10.992 INFO  [8231] [Lex::report@425] <OP>     INC '++'        Loc=<85:43>
2020-10-14 10:59:10.993 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<85:45>
2020-10-14 10:59:10.993 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<85:47>
2020-10-14 10:59:10.993 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<86:5>
2020-10-14 10:59:10.993 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<86:10>
2020-10-14 10:59:10.993 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<86:12>
2020-10-14 10:59:10.993 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<86:15>
2020-10-14 10:59:10.993 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<86:17>
2020-10-14 10:59:10.993 INFO  [8231] [Lex::report@425] <OP>     L_SQUARE '['    Loc=<86:21>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  i       Loc=<86:22>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_SQUARE ']'    Loc=<86:23>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<86:24>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  while   Loc=<87:5>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<87:11>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<87:12>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<87:16>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<87:18>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<88:7>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<88:12>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<88:14>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<88:18>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<89:7>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ASSIGN '='      Loc=<89:12>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  item    Loc=<89:14>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<89:18>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  hash_next       Loc=<89:20>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<89:29>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <RESERVED>       if      Loc=<90:7>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<90:10>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<90:11>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<90:15>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<90:17>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<90:20>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<90:22>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<91:9>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<91:12>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  key_free        Loc=<91:14>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<91:22>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<91:23>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<91:27>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  key     Loc=<91:29>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<91:32>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<91:33>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<92:7>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <RESERVED>       if      Loc=<93:7>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<93:10>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<93:11>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<93:15>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<93:17>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<93:21>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_BRACE '{'     Loc=<93:23>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<94:9>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<94:12>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  data_free       Loc=<94:14>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<94:23>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<94:24>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<94:28>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<94:30>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<94:34>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<94:35>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<95:7>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  free    Loc=<96:7>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<96:11>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  temp    Loc=<96:12>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<96:16>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<96:17>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<97:5>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<98:3>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  free    Loc=<99:3>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<99:7>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<99:8>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     ARROW '->'      Loc=<99:11>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  data    Loc=<99:13>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<99:17>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<99:18>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  free    Loc=<100:3>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     L_PAREN '('     Loc=<100:7>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <IDNET>  map     Loc=<100:8>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_PAREN ')'     Loc=<100:11>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     SEMICOLON ';'   Loc=<100:12>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@425] <OP>     R_BRACE '}'     Loc=<101:1>
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@429] 语句行数: 102
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@430] 字符总数: 2610
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@465] Token个数: 605
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@466] 其中OP个数: 324
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@467] 其中RESERVED个数: 49
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@468] 其中IDENT个数: 227
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@469] 其中NUMBER个数: 5
2020-10-14 10:59:10.996 INFO  [8231] [Lex::report@470] 其中STRING个数: 0
2020-10-14 10:59:10.997 INFO  [8231] [Lex::report@471] 其中CHAR个数: 0
```

### 错误的数字

#### 源代码

``` c
0x1111
0x111.1
0x111.1p2
12.1e1lu
12312
0999
01234
7890
0
09
```

#### 输出

``` shell
2020-10-14 11:09:49.119 INFO  [12255] [Lex::parse@421] <NUMBER> 0x1111  Loc=<1:1>
2020-10-14 11:09:49.125 WARN  [12255] [Lex::parse_number@69] the number 0x111.1 is not correct
2020-10-14 11:09:49.125 INFO  [12255] [Lex::parse@421] <NUMBER> 0x111.1 Loc=<2:1>
2020-10-14 11:09:49.131 INFO  [12255] [Lex::parse@421] <NUMBER> 0x111.1p2       Loc=<3:1>
2020-10-14 11:09:49.136 INFO  [12255] [Lex::parse@421] <NUMBER> 12.1e1lu        Loc=<4:1>
2020-10-14 11:09:49.142 INFO  [12255] [Lex::parse@421] <NUMBER> 12312   Loc=<5:1>
2020-10-14 11:09:49.148 WARN  [12255] [Lex::parse_number@69] the number 0999 is not correct
2020-10-14 11:09:49.148 INFO  [12255] [Lex::parse@421] <NUMBER> 0999    Loc=<6:1>
2020-10-14 11:09:49.153 INFO  [12255] [Lex::parse@421] <NUMBER> 01234   Loc=<7:1>
2020-10-14 11:09:49.159 INFO  [12255] [Lex::parse@421] <NUMBER> 7890    Loc=<8:1>
2020-10-14 11:09:49.165 INFO  [12255] [Lex::parse@421] <NUMBER> 0       Loc=<9:1>
2020-10-14 11:09:49.171 WARN  [12255] [Lex::parse_number@69] the number 09 is not correct
2020-10-14 11:09:49.171 INFO  [12255] [Lex::parse@421] <NUMBER> 09      Loc=<10:1>
2020-10-14 11:09:49.171 INFO  [12255] [Lex::report@431] 语句行数: 10
2020-10-14 11:09:49.171 INFO  [12255] [Lex::report@432] 字符总数: 60
2020-10-14 11:09:49.171 INFO  [12255] [Lex::report@467] Token个数: 10
2020-10-14 11:09:49.171 INFO  [12255] [Lex::report@468] 其中OP个数: 0
2020-10-14 11:09:49.171 INFO  [12255] [Lex::report@469] 其中RESERVED个数: 0
2020-10-14 11:09:49.171 INFO  [12255] [Lex::report@470] 其中IDENT个数: 0
2020-10-14 11:09:49.171 INFO  [12255] [Lex::report@471] 其中NUMBER个数: 10
2020-10-14 11:09:49.171 INFO  [12255] [Lex::report@472] 其中STRING个数: 0
2020-10-14 11:09:49.171 INFO  [12255] [Lex::report@473] 其中CHAR个数: 0
```
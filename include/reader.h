#pragma once
#include <cstddef>
#include <cstring>
#include <fstream>

const size_t READER_BUFFER = 1024;

struct Position {
  size_t row;
  size_t col;
};

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

private:
  std::ifstream file;
  char buffer[READER_BUFFER * 2];
  size_t index;
  size_t front_index;

  Position p_index;

  Position p_front_index;

  void read_buffer(char *buffer);
};

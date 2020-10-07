#include "reader.h"
#include <fstream>
#include <iostream>

Reader::Reader(const char *path)
    : index(0), front_index(1), p_index(Position{1, 1}),
      p_front_index(Position{1, 2}) {
  this->file = std::ifstream(path);
  memset(this->buffer, 0, 2 * READER_BUFFER);
  this->read_buffer(buffer);
}

void Reader::ahead() {
  this->index = this->front_index;
  this->p_index.col = this->p_front_index.col;
  this->p_index.row = this->p_front_index.row;
  this->front_ahead();
}

void Reader::front_ahead() {
  this->front_index = (this->front_index + 1) % (READER_BUFFER * 2);
  if (this->front_index == 0) {
    this->read_buffer(this->buffer);
    memset(this->buffer + READER_BUFFER, 0, READER_BUFFER);
  } else if (this->front_index == READER_BUFFER) {
    this->read_buffer(this->buffer + READER_BUFFER);
    memset(this->buffer, 0, READER_BUFFER);
  }
  if (this->front_peek() == '\n') {
    this->p_front_index.row += 1;
    this->p_front_index.col = 0;
  } else {
    this->p_front_index.col += 1;
  }
}

char Reader::peek() const { return this->buffer[this->index]; }

char Reader::front_peek() const { return this->buffer[this->front_index]; }

bool Reader::is_eof() const { return this->file.eof(); }

void Reader::read_buffer(char *buffer) {
  this->file.read(buffer, READER_BUFFER);
}

Position Reader::pos() const { return this->p_index; }
#include "reader.h"
#include <fstream>
#include <iostream>

Reader::Reader(const char *path) : index(0), front_index(1) {
  this->file = std::ifstream(path);
  memset(this->buffer, 0, 2 * READER_BUFFER);
  this->read_buffer(buffer);
}

void Reader::ahead() {
  this->index = this->front_index;
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
}

char Reader::peek() const { return this->buffer[this->index]; }

char Reader::front_peek() const { return this->buffer[this->front_index]; }

bool Reader::is_eof() const { return this->file.eof(); }

void Reader::read_buffer(char *buffer) {
  this->file.read(buffer, READER_BUFFER);
}

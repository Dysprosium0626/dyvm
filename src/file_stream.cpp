//
// Created by Dysprosium on 2023/3/1.
//

#include "file_stream.hpp"
#include <iostream>

namespace dyvm {
FileStream::FileStream(std::string path) : path_(path) {
};

FileStream::~FileStream() {
  Close();
}

// Use a while-loop to read from the file until the end of the file.
std::string FileStream::Read() {
  char buffer[MAX_BUFFER_SIZE];
  while (!stream_.eof()) {
    stream_.getline(buffer, MAX_BUFFER_SIZE);
    std::cout << buffer << std::endl;
  }
  return buffer;

}

// Once the file is in the open state. Write the content into the file.
void FileStream::Write(std::string content) {
  if (stream_.is_open()) {
    stream_ << content;
  }
}

// Open the file with given path and exit once the file is incorrect.
void FileStream::Open() {
  stream_.open(path_);
  if (!stream_.is_open()) {
    std::cout << "Fail to open the file" << std::endl;
    exit(-1);
  }
}

// Close the file stream
void FileStream::Close() {
  stream_.close();
}
} // dyvm


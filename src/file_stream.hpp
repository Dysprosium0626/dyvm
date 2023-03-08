//
// Created by Dysprosium on 2023/3/1.
//

#ifndef DYVM_SRC_FILE_STREAM_HPP_
#define DYVM_SRC_FILE_STREAM_HPP_
# include <string>
#include <fstream>
namespace dyvm {
class FileStream {
 public:
  // Create a new FileStream based on fstream.
  // The "path" is used to indicate the relative path of the file to be operated.
  explicit FileStream(std::string path);

  // The default destructor of FileStream object.
  // It will automatically close a stream.
  ~FileStream();

  // Read the content of the file.
  std::string Read();

  // Write the certain string (parameter "content") into the file.
  void Write(std::string content);

  // Open the file according to the given path.
  // It will be closed once the path is incorrect.
  void Open();

  // Close the file stream.
  void Close();
 private:
  enum {
    MAX_BUFFER_SIZE = 100
  };
  std::fstream stream_;
  std::string path_;

};
} // dyvm


#endif //DYVM_SRC_FILE_STREAM_HPP_

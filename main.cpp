#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>
#include "src/nfa.hpp"
#include "src/file_stream.hpp"

int main() {
  std::string path = "../test/regex_input.txt";
  dyvm::FileStream file(path);
  file.Open();
  auto regex = file.Read();
  dyvm::NFA nfa;
  auto result = nfa.RegexToNFA(regex);
  result.Print();

  return 0;
}
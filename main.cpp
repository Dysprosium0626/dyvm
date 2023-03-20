#include <iostream>
#include <stack>
#include "src/nfa.hpp"
#include "src/file_stream.hpp"
#include "src/dfa.hpp"
int main() {
  std::string path = "../test/regex_input.txt";
  dyvm::FileStream file(path);
  file.Open();
  auto regex = file.Read();
  dyvm::NFA nfa;
  auto result = nfa.RegexToNFA(regex);
  result.Print();
  dyvm::DFA dfa(result);
  auto res = dfa.EpsilonEnclosure(result.start);
  for (const auto &item : res.states) {
    std::cout << item->id << '\t' << std::endl;
  }

  dfa.ConvertNFAToDFA();
  for (const auto &d : dfa.states) {
    std::cout << d.id <<' ' << &d << ' ' << "accepted " << d.accepted << ' ';
    for (const auto &t : d.transitions) {
      std::cout << t.first << ' ';
      for (const auto &item : t.second) {
        std::cout << item.id << ' ' << &item;
      }
    }
    std::cout << std::endl;
  }
  dfa.MinimizeDFA();
  std::cout << "After minimize:" << std::endl;

  for (const auto &d : dfa.states) {
    std::cout << d.id <<' ' << &d << ' ' << "accepted " << d.accepted << ' ';
    for (const auto &t : d.transitions) {
      std::cout << t.first << ' ';
      for (const auto &item : t.second) {
        std::cout << item.id << ' ' << &item << ' ';
      }
    }
    std::cout << std::endl;
  }


}
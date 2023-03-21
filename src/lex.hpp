//
// Created by Dysprosium on 2023/3/20.
//

#ifndef DYVM_SRC_LEX_HPP_
#define DYVM_SRC_LEX_HPP_

#include "dfa.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
namespace dyvm {

class Lex {
 public:
  Lex() {
    std::ifstream f(regex_path);
    nlohmann::json data = nlohmann::json::parse(f);
    for (const auto &[key, value] : data.items()) {
      std::cout << "identifier : " << key << std::endl;
      std::cout << "value : " << value << std::endl;
      NFA nfa;;
      auto nfa1 = nfa.RegexToNFA(value);
      DFA dfa(nfa1);
      dfa.ConvertNFAToDFA();
      std::cout << "start_ids" << ' ';
      for (const auto &item : dfa.start_ids) {
        std::cout << item << ' ';
      }
      std::cout << std::endl;
      dfa.MinimizeDFA();
      std::cout << "After minimize:" << std::endl;
      std::cout << "start_ids" << ' ';
      for (const auto &item : dfa.start_ids) {
        std::cout << item << ' ';
      }
      std::cout << std::endl;
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
      dfa_map.insert(std::make_pair(key, dfa));
    }
    std::cout << "Conversion ends" << std::endl;
  }
  void Analyze();
  bool Match(const std::string key, std::string line, int &x, DFAState dfa_state, DFA dfa);
 public:
  std::string file_path="../test/test_input.txt";
  std::string regex_path = "../test/lex.json";
  std::vector<std::string> tokens;
  std::map<std::string , DFA> dfa_map;
};

} // dyvm

#endif //DYVM_SRC_LEX_HPP_

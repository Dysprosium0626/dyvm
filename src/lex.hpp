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
      if (key == "identifier") {
        DFA identifier_dfa = DFA();
        identifier_dfa.start_ids.push_back(0);

        DFAState start_state = DFAState(1);
        start_state.accepted = true;
        for (int i = 'a'; i <= 'z'; ++i) {
          start_state.AddTransition(i, DFAState(1, true));
        }
        for (int i = 'A'; i <= 'Z'; ++i) {
          start_state.AddTransition(i, DFAState(1, true));
        }
        start_state.AddTransition('_', DFAState(1, true));

        DFAState zero_state = DFAState(0);
        zero_state.accepted = false;
        for (int i = 'a'; i < 'z'; ++i) {
          zero_state.AddTransition(i, start_state);
        }
        for (int i = 'A'; i < 'Z'; ++i) {
          zero_state.AddTransition(i, start_state);
        }
        zero_state.AddTransition('_', start_state);
        identifier_dfa.states.push_back(zero_state);
        identifier_dfa.states.push_back(start_state);
        dfa_map.insert(std::make_pair("identifier", identifier_dfa));
        continue;
      }
      NFA nfa;;
      auto nfa1 = nfa.RegexToNFA(value);
      DFA dfa(nfa1);
      dfa.ConvertNFAToDFA();
      dfa.MinimizeDFA();
      std::cout << "After minimize:" << std::endl;
      std::cout << "start_ids" << ' ';
      for (const auto &item : dfa.start_ids) {
        std::cout << item << ' ';
      }
      std::cout << std::endl;
      for (const auto &d : dfa.states) {
        std::cout << d.id << ' ' << "accepted " << d.accepted << ' ';
        for (const auto &t : d.transitions) {
          std::cout << t.first << ' ';
          for (const auto &item : t.second) {
            std::cout << item.id << ' ';
          }
        }
        std::cout << std::endl;
      }
      dfa_map.insert(std::make_pair(key, dfa));
    }
    std::cout << "Conversion ends" << std::endl;

  }

  void Analyze();
  bool Match(const std::string &key, std::string line, int &x, const DFAState &dfa_state, DFA dfa);
 public:
  std::string file_path = "../test/test_input.txt";
  std::string regex_path = "../test/lex.json";
  std::vector<std::string> tokens;
  std::map<std::string, DFA> dfa_map;
};





} // dyvm

#endif //DYVM_SRC_LEX_HPP_

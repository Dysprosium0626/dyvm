//
// Created by Dysprosium on 2023/3/20.
//

#include "lex.hpp"
#include <fstream>

namespace dyvm {

enum {
  MAX_BUFFER_SIZE = 2049
};

DFAState GetDFAState(std::vector<DFAState> states, int id) {
  return states[id];
}

bool Lex::Match(const std::string &key, std::string line, int &x, const DFAState &dfa_state, DFA dfa) {
  bool is_match = false;
  for (const auto &transitions : dfa_state.transitions) {
    if (line[x] == transitions.first) {
      is_match = true;
      for (const auto &item : transitions.second) {
        auto next = GetDFAState(dfa.states, item.id);
        return Match(key, line, ++x, next, dfa);
      }
    }
  }
  if (!is_match) {
    if (dfa_state.accepted) {
      tokens.push_back(key);
      return true;
    }
    return false;
  }
}

void Lex::Analyze() {
  std::fstream stream_;
  stream_.open(file_path);
  char buffer[MAX_BUFFER_SIZE];
  while (!stream_.eof()) {
    stream_.getline(buffer, MAX_BUFFER_SIZE);
    auto line = std::string(buffer);
    int x = 0;
    while (x < line.size()) {
      for (const auto &[key, dfa] : dfa_map) {
        std::cout << key << std::endl;
        auto start = dfa.start_ids[0];
        DFAState start_state = GetDFAState(dfa.states, start);
        bool transition_match = false;
        int origin_x = x;
        for (const auto &transition : start_state.transitions) {
          if (transition.first == line[x]) {
            transition_match = Match(key, line, x, start_state, dfa);
            break;
          }
        }
        if (!transition_match) {
          x = origin_x;
          std::cout << "fail" << std::endl;
        } else {
          std::cout << "success" << std::endl;
          break;
        }
      }
    }
  }
  std::cout << "Result: ";
  for (const auto &item : tokens) {
    std::cout << item << ' ';
  }

}
} // dyvm
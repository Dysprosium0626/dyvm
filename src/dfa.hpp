//
// Created by Dysprosium on 2023/3/8.
//

#ifndef DYVM_SRC_DFA_HPP_
#define DYVM_SRC_DFA_HPP_
#include "nfa.hpp"
#include <vector>
#include <unordered_set>
namespace dyvm {

class DFAState {
 public:
  explicit DFAState() {};
  explicit DFAState(int id) : id(id) {};
  explicit DFAState(const std::unordered_set<std::shared_ptr<State>> &states) : states(states) {
  }
  int id;
  std::unordered_set<std::shared_ptr<State>> states;
  std::unordered_map<char, std::vector<DFAState>> transitions;

  void AddTransition(char input, DFAState next_state) {
    if (transitions.find(input) == transitions.end()) {
      transitions[input] = std::vector<DFAState>();
    }
    transitions[input].push_back(next_state);
  }

};

class DFATransition {
 public:
  // Input character
  char input;

  // Next states of a transition
  std::vector<DFAState> next_states;

  // Add a new next state for the transition
  void AddNextState(DFAState state) {
    next_states.push_back(state);
  }
};

class DFA : NFA {
 public:
  DFA() {};
  DFA(NFA &nfa) : NFA(nfa) {
//    ConvertNFAToDFA();
  };
  std::shared_ptr<DFAState> d_start;
  std::vector<DFAState> states;

  // To covert NFA to DFA by subset construction algorithm
  void ConvertNFAToDFA();
  DFAState EpsilonEnclosure(std::shared_ptr<State> state, char input = 'E');

};

} // dyvm

#endif //DYVM_SRC_DFA_HPP_

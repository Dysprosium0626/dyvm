//
// Created by Dysprosium on 2023/3/7.
//

#ifndef DYVM_SRC_NFA_HPP_
#define DYVM_SRC_NFA_HPP_
#include <set>
#include <vector>
#include <stack>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace dyvm {

// The state of a nfa
class State {
 public:
  // The id of the state in numeric order starts from zero
  int id;

  // Transition functions of the state
  std::unordered_map<char, std::vector<std::shared_ptr<State>>> transitions;

  explicit State(int id) : id(id) {}

  // Add a new transition with given input and next state into the transitions map
  void AddTransition(char input, std::shared_ptr<State> next_state) {
    if (transitions.find(input) == transitions.end()) {
      transitions[input] = std::vector<std::shared_ptr<State>>();
    }
    transitions[input].push_back(next_state);
  }
};

// The transition of a nfa
class Transition {
 public:
  // Input character
  char input;

  // Next states of a transition
  std::unordered_set<std::shared_ptr<State>> next_states;

  explicit Transition(char input) : input(input) {}
  explicit Transition(const std::unordered_set<std::shared_ptr<State>> &next_states) : next_states(next_states) {}

  // Add a new next state for the transition
  void AddNextState(std::shared_ptr<State> state) {
    next_states.insert(state);
  }
};

class NFA {
 public:

  // The start state of the nfa
  std::shared_ptr<State> start;

  // The accept state of the nfa
  std::shared_ptr<State> accept;
  NFA() {}
  NFA(const std::shared_ptr<State> &start, const std::shared_ptr<State> &accept) : start(start), accept(accept) {}

  // Print the nfa from start state to accept state
  void Print();

  // Generate a new state with the calculation of the given operator
  void PopCalculate(std::stack<char> &s, std::stack<NFA> &nfa_stack, int &state_id);

  // Convert regex to nfa
  NFA RegexToNFA(std::string regex);

 private:
  // Concat operator
  NFA Concat(NFA nfa1, NFA nfa2, int &state_id);

  // Or operator
  NFA Or(NFA nfa1, NFA nfa2, int state_id);

  // Closure operator
  NFA Closure(NFA nfa1, int &state_id);

  // Single character
  NFA SingleChar(char c, int &state_id);

  NFA Hyphen(NFA nfa1, NFA nfa2, int &state_id);

};

} // dyvm


#endif //DYVM_SRC_NFA_HPP_

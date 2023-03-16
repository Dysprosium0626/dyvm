//
// Created by Dysprosium on 2023/3/7.
//

#include "nfa.hpp"
#include <stack>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
namespace dyvm {

// The statically initialized map is used to compare the priority of symbol in [ |, ., *]
static const std::unordered_map<char, int> priority_{
    {'|', 1},
    {'.', 2},
    {'*', 3}
};

void NFA::Print() {
  std::cout << "NFA Transitions:" << std::endl;
  std::unordered_set<std::shared_ptr<State>> visited;
  std::stack<std::shared_ptr<State>> s;
  s.push(start);
  visited.insert(start);

  while (!s.empty()) {
    auto current_state = s.top();
    s.pop();
    for (auto &transition : current_state->transitions) {
      for (auto &dest : transition.second) {
        std::cout << "State " << current_state->id << " -> State " << dest->id << " on input " << transition.first
                  << std::endl;
        if (visited.find(dest) == visited.end()) {
          s.push(dest);
          visited.insert(dest);
        }
      }
    }
  }
  std::cout << std::endl;
}

void NFA::PopCalculate(std::stack<char> &s, std::stack<NFA> &nfa_stack, int &state_id) {
  char op = s.top();
  s.pop();  // Pop up the operator stack
  // Pop up the last two state in the vector and conduct calculation.
  if (op == '|') {
    NFA y = nfa_stack.top();
    nfa_stack.pop();
    NFA x = nfa_stack.top();
    nfa_stack.pop();
    NFA or_nfa = Or(x, y, state_id);
    nfa_stack.push(or_nfa);// Push the result into the state vector.
  } else if (op == '.') {
    NFA y = nfa_stack.top();
    nfa_stack.pop();
    NFA x = nfa_stack.top();
    nfa_stack.pop();
    NFA concat_nfa = Concat(x, y, state_id);
    nfa_stack.push(concat_nfa);   // Push the result into the state vector.
  } else if (op == '*') {
    // TODO check the correctness of star operator
    NFA x = nfa_stack.top();
    nfa_stack.pop();
    NFA closure_nfa = Closure(x, state_id);
    nfa_stack.push(closure_nfa);  // Push the result into the number vector.
  } else {
    std::cerr << "Unrecognized operator" << std::endl;
  }
}
NFA NFA::RegexToNFA(std::string regex) {
  std::stack<char> s;
  std::stack<NFA> nfa_stack;
  int state_id = 0;
  for (const auto &c : regex) {
    if (c == '(') {
      s.push(c);
    } else if (c == ')') {
      if (s.empty()) {
        break;
      } else {
        while (s.top() != '(') {
          PopCalculate(s, nfa_stack, state_id);
          if (s.empty()) {
            break;
          }
        }
        s.pop();
      }
    } else if (c == '.') {
      while (!s.empty() && s.top() != '('
          && priority_.find(s.top())->second >= priority_.find(c)->second) {
        PopCalculate(s, nfa_stack, state_id);
      }
      s.push(c);
    } else if (c == '|') {
      while (!s.empty() && s.top() != '('
          && priority_.find(s.top())->second >= priority_.find(c)->second) {
        PopCalculate(s, nfa_stack, state_id);
      }
      s.push(c);
    } else if (c == '*') {
      while (!s.empty() && s.top() != '('
          && priority_.find(s.top())->second >= priority_.find(c)->second) {
        PopCalculate(s, nfa_stack, state_id);
      }
      s.push(c);
    } else {
      NFA single_char_nfa = SingleChar(c, state_id);
      nfa_stack.push(single_char_nfa);
    }
  }
  while (not s.empty()) {
    PopCalculate(s, nfa_stack, state_id);
  }
  auto result = nfa_stack.top();
  return result;

}
NFA NFA::Concat(NFA nfa1, NFA nfa2, int &state_id) {
  //
  // start_state -----> nfa1 -----> nfa2 -----> accept_state
  //
  nfa1.accept->AddTransition('E', nfa2.start);
  NFA concat_nfa(nfa1.start, nfa2.accept);
  return concat_nfa;
}
NFA NFA::Or(NFA nfa1, NFA nfa2, int state_id) {
  //                    --- nfa1 -->
  //                  /             \
  // start_state----->                ----->  accept_state
  //                  \             /
  //                    --- nfa2 -->
  auto start_state = std::make_shared<State>(state_id++);
  start_state->AddTransition('E', nfa1.start);
  start_state->AddTransition('E', nfa2.start);

  auto accept_state = std::make_shared<State>(state_id++);
  nfa1.accept->AddTransition('E', accept_state);
  nfa2.accept->AddTransition('E', accept_state);

  NFA or_nfa(start_state, accept_state);
  return or_nfa;
}
NFA NFA::Closure(NFA nfa1, int &state_id) {
  // start_state -----> nfa1 -----> accept_state
  //               |           |
  //                <----------
  auto start_state = std::make_shared<State>(state_id++);
  start_state->AddTransition('E', nfa1.start);
  nfa1.accept->AddTransition('E', start_state);

  auto accept_state = std::make_shared<State>(state_id++);
  nfa1.accept->AddTransition('E', accept_state);
  start_state->AddTransition('E', accept_state);

  NFA closure_nfa(start_state, accept_state);
  return closure_nfa;
}
NFA NFA::SingleChar(char c, int &state_id) {
  // Just generator a new state with the given character and state id
  auto start_state = std::make_shared<State>(state_id++);
  auto accept_state = std::make_shared<State>(state_id++);
  start_state->AddTransition(c, accept_state);
  NFA single_char_nfa(start_state, accept_state);
  return single_char_nfa;
}

} // dyvm
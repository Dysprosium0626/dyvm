//
// Created by Dysprosium on 2023/3/8.
//

#include "dfa.hpp"
#include <unordered_set>

namespace dyvm {

void DFA::ConvertNFAToDFA() {
  auto dfa_state = EpsilonEnclosure(start);

  std::unordered_set<std::shared_ptr<DFAState>> visited;
  std::vector<DFAState> to_visit;

  // Init the visit array with the epsilon enclosure of the start state
  to_visit.push_back(dfa_state);
  visited.insert(std::make_shared<DFAState>(dfa_state));
  while (!to_visit.empty()) {
    // Get a state from the array
    auto current_state = to_visit.back();
    to_visit.pop_back();


    for (const auto &state : current_state.states) {
      for (const auto &transition : state->transitions) {
        auto input = transition.first;
        auto next_states = transition.second;
        std::unordered_set<std::shared_ptr<State>> next_states_set;
        for (const auto &next_state : next_states) {
          auto enclosure = EpsilonEnclosure(next_state);
          next_states_set.insert(enclosure.states.begin(), enclosure.states.end());
        }
        DFAState next_dfa_state(next_states_set);
        current_state.AddTransition(input, next_states_set);

        if (visited.count(std::make_shared<DFAState>(next_dfa_state)) == 0) {
          for (const auto &i : next_dfa_state.states) {
            std::cout << i->id << ' ';
          }
          std::cout << std::endl;
        }
        visited.insert(std::make_shared<DFAState>(next_dfa_state));
        to_visit.push_back(next_dfa_state);
      }
    }
  }

}

DFAState DFA::EpsilonEnclosure(std::shared_ptr<State> state, char input) {
  DFAState visited;
  if (auto result = visited.states.find(state); result == visited.states.end()) {
    visited.states.insert(state);
  }
  for (const auto &transition : state->transitions) {
    if (transition.first == input) {
      for (const auto &next_state : transition.second) {
        if (auto result = visited.states.find(next_state); result == visited.states.end()) {
          visited.states.insert(next_state);
          auto sub_enclosure = EpsilonEnclosure(next_state, input);
          visited.states.insert(sub_enclosure.states.begin(), sub_enclosure.states.end());
        }
      }
    }
  }
  return visited;
}
} // dyvm
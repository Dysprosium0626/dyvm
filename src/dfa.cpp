//
// Created by Dysprosium on 2023/3/8.
//

#include "dfa.hpp"
#include <unordered_set>

namespace dyvm {

bool SameSet(std::unordered_set<int> &s1, std::unordered_set<int> &s2) {
  if (s1.size() != s2.size()) {
    return false;
  }

  std::unordered_set<int> intersect;
  for (auto x : s1) {
    if (s2.count(x) > 0) {
      intersect.insert(x);
    }
  }

  return intersect.size() == s1.size();
}

bool TwoSetExist(std::unordered_set<std::shared_ptr<DFAState>> visited,
                 DFAState &dfa) {
  auto set = dfa.states;
  bool result = false;

  std::unordered_set<int> id_set;
  for (const auto &item : set) {
    id_set.insert(item->id);
  }

  for (const auto &dfa_state : visited) {
    std::unordered_set<int> visited_set;
    auto id = dfa_state->id;
    for (const auto &state : dfa_state->states) {
      visited_set.insert(state->id);
    }
    if (SameSet(id_set, visited_set)) {
      result = true;
      dfa.id = id;
    }
  }
  return result;
}

// TODO: assign an id to dfa_state
void DFA::ConvertNFAToDFA() {
  auto dfa_state = EpsilonEnclosure(start);
  int dfa_state_id = 0;
  dfa_state.id = dfa_state_id;
  std::unordered_set<std::shared_ptr<DFAState>> visited;
  std::vector<DFAState> to_visit;

  // Init the visit array with the epsilon enclosure of the start state
  to_visit.push_back(dfa_state);
  visited.insert(std::make_shared<DFAState>(dfa_state));
  while (!to_visit.empty()) {
    // Get a state from the array
    auto current_state = to_visit.back();
    to_visit.pop_back();
    std::cout << "current state: ";
    for (const auto &item : current_state.states) {
      std::cout << item->id << ' ';
    }
    std::cout << std::endl;
    for (const auto &state : current_state.states) {
      for (const auto &transition : state->transitions) {
        auto input = transition.first;
        if (input == 'E') {
          continue;
        }
        auto next_states = transition.second;

        std::unordered_set<std::shared_ptr<State>> next_states_set;
        for (const auto &next_state : next_states) {
          auto enclosure = EpsilonEnclosure(next_state);
          next_states_set.insert(enclosure.states.begin(), enclosure.states.end());
        }

        DFAState next_dfa_state(next_states_set);
        if (!TwoSetExist(visited, next_dfa_state)) {
          next_dfa_state.id = visited.size();
          visited.insert(std::make_shared<DFAState>(next_dfa_state));
          to_visit.push_back(next_dfa_state);
        }
        current_state.AddTransition(input, next_dfa_state);
        std::cout << "input: " << input << ' ';
        for (const auto &i : next_dfa_state.states) {
          std::cout << i->id << ' ';
        }
        std::cout << std::endl;

      }
    }
    states.push_back(current_state);
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
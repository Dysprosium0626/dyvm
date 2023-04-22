//
// Created by Dysprosium on 2023/3/8.
//

#include "dfa.hpp"
#include <unordered_set>
#include <queue>
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

bool TwoSetExist(const std::unordered_set<std::shared_ptr<DFAState>> &visited,
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

void DFA::ConvertNFAToDFA() {
  auto start_state = EpsilonEnclosure(start);
  int start_state_id = 0;
  start_state.id = start_state_id;
  auto nfa_start_id = start->id;
  std::unordered_set<std::shared_ptr<DFAState>> visited;
  std::vector<DFAState> to_visit;

  // Init the visit array with the EPSILON enclosure of the start state
  to_visit.push_back(start_state);
  visited.insert(std::make_shared<DFAState>(start_state));

  while (!to_visit.empty()) {
    // Get a state from the array
    auto current_state = to_visit.back();
    to_visit.pop_back();
    std::unordered_map<char, std::unordered_set<std::shared_ptr<State>>> temp;
    for (const auto &state : current_state.states) {
      for (const auto &transition : state->transitions) {
        auto input = transition.first;
        if (input == '\0') {
          continue;
        } else {
          alphabet.insert(input);
        }
        auto next_states = transition.second;
        std::unordered_set<std::shared_ptr<State>> next_states_set;
        for (const auto &next_state : next_states) {
          auto enclosure = EpsilonEnclosure(next_state);
          next_states_set.insert(enclosure.states.begin(), enclosure.states.end());
        }
        temp[input].insert(next_states_set.begin(), next_states_set.end());
      }
    }
    for (const auto &[input, set] : temp) {
      DFAState next_dfa_state(set);
      if (!TwoSetExist(visited, next_dfa_state)) {
        next_dfa_state.id = visited.size();
        visited.insert(std::make_shared<DFAState>(next_dfa_state));
        to_visit.push_back(next_dfa_state);
      }
      current_state.AddTransition(input, next_dfa_state);
    }
    states.push_back(current_state);
  }
  for (auto &item : states) {
    for (const auto &state : item.states) {
      if (state->id == accept->id) {
        item.accepted = true;
      }
    }
  }
  for (auto &item : states) {
    for (const auto &state : item.states) {
      if (state->id == nfa_start_id) {
        start_ids.push_back(item.id);
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

DFAState GetCertainDFAState(std::vector<DFAState> states, int id) {
  for (const auto &state : states) {
    if (state.id == id) {
      return state;
    }
  }
}
int GetGroupById(std::deque<std::vector<int>> worklist, int id) {
  for (int i = 0; i < worklist.size(); ++i) {
    if (auto res = std::find(worklist[i].begin(), worklist[i].end(), id); res != worklist[i].end()) {
      return i;
    }
  }
}

void DFA::MinimizeDFA() {
  std::vector<std::vector<int>> partitions = {{}, {}};
  std::vector<int> mini_start_ids;
  for (const auto &state : states) {
      partitions[state.accepted].push_back(state.id);
  }

  std::deque<std::vector<int>> worklist;
  for (const auto &partition : partitions) {
    worklist.push_back(partition);
  }
  int unchanged = 0;
  int count = 0;
  while (!worklist.empty()) {
    int size_before = worklist.size();
    auto partition = worklist.front();
    std::vector<int> image;
    std::vector<int> diff;
    auto group_id = 0;
    for (const auto &id : partition) {
      auto state = GetCertainDFAState(states, id);
      int t_count = 0;
      int reflection = 0;
      for (const auto &c : alphabet) {
        auto transition = state.transitions[c];
        if (!transition.empty()) {
          for (const auto &t : transition) {
            t_count++;
            if (GetGroupById(worklist, t.id) == group_id) {
              reflection++;
            }
          }
        }
      }
      if (reflection == t_count) {
        image.push_back(id);
      } else {
        diff.push_back(id);
      }
    }
    worklist.pop_front();
    if(image.size()) {
      worklist.push_back(image);
    }
    if (diff.size()) {
      worklist.push_back(diff);
    }
    count++;

    if (worklist.size() != size_before) {
      unchanged = 0;
    } else {
      unchanged++;
    }
    if (unchanged == worklist.size()) {
      break;
    }
  }

  // Refine new states
  std::vector<DFAState> minimized_states;
  for (int i = 0; i < worklist.size(); ++i) {
    // Init all states
    auto partition = worklist[i];
    DFAState state;
    state.id = i;
    bool started = false;
    for (const auto &item : partition) {
      auto dfa = GetCertainDFAState(states, item);
      if (dfa.accepted) {
        state.accepted = true;
        break;
      }
      if (auto res = std::find(start_ids.begin(), start_ids.end(), item); res != start_ids.end() && !started) {
        mini_start_ids.push_back(state.id);
        started = true;
      }
    }
    minimized_states.push_back(state);
  }
  // Add transitions
  // TODO: Timeout
  for (auto &state : minimized_states) {
    auto partition = worklist[state.id];
    for (const auto &id : partition) {
      auto dfa_state = GetCertainDFAState(states, id);
      auto transitions = dfa_state.transitions;
      for (const auto &[input, state_ids] : transitions) {
        for (const auto &item : state_ids) {
          for (int i = 0; i < worklist.size(); ++i) {
            if (auto res = std::find(worklist[i].begin(), worklist[i].end(), item.id); res != worklist[i].end()) {
              auto group_id = i;
              if (state.transitions[input].empty()) {
                state.AddTransition(input, minimized_states[group_id]);
              }
            }
          }
        }
      }
    }
  }
  states = minimized_states;
  start_ids = mini_start_ids;
}

} // dyvm
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

bool TwoSetExist(const std::unordered_set<std::shared_ptr<DFAState>>& visited,
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
  d_start = std::make_shared<DFAState>(dfa_state);
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
        } else {
          alphabet.insert(input);
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
  for (auto &item : states) {
    for (const auto &state : item.states) {
      if (state->id == accept->id) {
        item.accepted = true;
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
      return i+1;
    }
  }
}

void DFA::MinimizeDFA() {
  std::vector<std::vector<int>> partitions = {{}, {}};
  for (const auto &state : states) {
    partitions[state.accepted].push_back(state.id);
  }

  std::deque<std::vector<int>> worklist;
  for (const auto &partition : partitions) {
      worklist.push_back(partition);
  }
  int unchanged = 0;
  while (!worklist.empty()) {
    int size_before = worklist.size();
    auto partition = worklist.front();
    std::unordered_map<int, std::vector<int>> partition_map;
    for (const auto &id : partition) {
      auto state = GetCertainDFAState(states, id);
      auto hash_id = 0b0;
      for (const auto &c : alphabet) {
        auto transition = state.transitions[c];
        if (transition.empty()) {
          hash_id = 0b0 | (hash_id << 0b1);
          continue;
        } else {
          auto group_id = GetGroupById(worklist, id);
          hash_id = group_id | (hash_id << (static_cast<int>(log2(group_id)) + 1));
        }
      }
      if (auto res = partition_map.find(hash_id); res != partition_map.end()) {
        res->second.push_back(id);
      } else {
        partition_map.insert(std::make_pair(hash_id, std::vector<int>{id}));
      }
    }
    worklist.pop_front();
    for (const auto &[_, par] : partition_map) {
      worklist.push_back(par);
    }
    if(worklist.size() != size_before) {
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
    for (const auto &item : partition) {
      auto dfa = GetCertainDFAState(states, item);
      if(dfa.accepted) {
        state.accepted = true;
        break;
      }
    }
    minimized_states.push_back(state);
  }
  // Add transitions
  for (auto &state : minimized_states) {
    auto partition = worklist[state.id];
    for (const auto &id : partition) {
      auto transitions = GetCertainDFAState(states, id).transitions;
      for (const auto &[input, state_ids] : transitions) {
        for (const auto &item : state_ids) {
          for (int i = 0; i < worklist.size(); ++i) {
            if(auto res = std::find(worklist[i].begin(), worklist[i].end(), item.id); res != worklist[i].end()) {
              auto group_id = i;
              state.AddTransition(input, minimized_states[group_id]);
            }
          }
        }
      }
    }
  }
  states = minimized_states;
}

} // dyvm
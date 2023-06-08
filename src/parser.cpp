//
// Created by Dysprosium on 2023/4/6.
//
#include "parser.hpp"
#include <stack>
#include <set>

void dyvm::LL1::Preprocess() {
  // Eliminate left recursive
  for (auto &[left, right] : productions) {
    std::vector<std::vector<size_t>> alpha_productions;
    std::vector<std::vector<size_t>> beta_productions;
    bool left_recursive = false;
    std::string a_prime;
    size_t a_prime_id;
    std::vector<std::vector<size_t>> alphas;
    std::vector<std::vector<size_t>> betas;
    for (const auto &item : right) {
      std::vector<size_t> alpha;
      std::vector<size_t> beta;
      if (item[0] == left) {
        left_recursive = true;
        a_prime = FindKey(item[0]) + "'";
        if (auto res = materialization.find(a_prime); res != materialization.end()) {
          a_prime_id = res->second;
        } else {
          a_prime_id = tokens.size();
          materialization.insert(std::make_pair(a_prime, a_prime_id));
          non_terminals.insert(a_prime_id);
        }
        tokens.insert(a_prime);
        for (int i = 1; i < item.size(); ++i) {
          alpha.push_back(item[i]);
        }
      } else {
        for (unsigned long i : item) {
          beta.push_back(i);
        }
      }
      if (!alpha.empty()) {
        alphas.push_back(alpha);
      } else if (!beta.empty()) {
        betas.push_back(beta);
      }
    }
    if (left_recursive) {
      for (auto &item : betas) {
        item.push_back(a_prime_id);
      }
      for (auto &item : alphas) {
        item.push_back(a_prime_id);
      }
      alphas.push_back(std::vector<size_t>{EPSILON});
      productions[a_prime_id] = alphas;
      productions[left] = betas;
    } else {
      continue;
    }
  }
  std::cout << "tokens: " << std::endl;
  for (const auto &item : tokens) {
    std::cout << item << std::endl;
  }

  std::cout << "terminals: " << std::endl;
  for (const auto &item : terminals) {
    std::cout << item << std::endl;
  }

  std::cout << "non_terminals: " << std::endl;
  for (const auto &item : non_terminals) {
    std::cout << item << std::endl;
  }

  std::cout << "materialization: " << std::endl;
  for (const auto &[k, v] : materialization) {
    std::cout << "item:" << k << " " << v << std::endl;
  }

  for (const auto &[nt, symbols] : productions) {
    std::cout << "left:" << nt << std::endl;
    std::cout << "right:" << std::endl;
    for (const auto &symbol : symbols) {
      for (const auto &item : symbol) {
        std::cout << item << ' ';
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

}
void dyvm::LL1::GetFirst() {
  bool changed = true;
  while (changed) {
    changed = false;
    for (const auto &[left, right] : productions) {
      if (IsNonTerminal(left)) {
        std::unordered_set<size_t> non_terminal_first;
        if (auto res = first.find(left); res != first.end()) {
          non_terminal_first = first[left];
        }
        for (const auto &i : right) {
          for (unsigned long j : i) {
            bool no_epsilon = true;
            if (IsTerminal(j) or j == EPSILON) {
              if (auto res = non_terminal_first.find(j); res
                  == non_terminal_first.end()) {
                if (j == EPSILON) {
                  no_epsilon = false;
                }
                changed = true;
                non_terminal_first.insert(j);
              }
            }
            if (auto res = first.find(j); res != first.end()) {
              for (const auto &item : res->second) {
                if (item != EPSILON) {
                  if (auto find = non_terminal_first.find(item); find
                      == non_terminal_first.end()) {
                    changed = true;
                    non_terminal_first.insert(item);
                  }
                }
              }
            }
            if (no_epsilon) {
              break;
            }
          }
        }
        first[left] = non_terminal_first;
      }
//      std::cout << "first" << std::endl;
//      for (const auto &[k, v] : first) {
//        std::cout << "left " << k << std::endl;
//        std::cout << "right ";
//        for (const auto &item : v) {
//          std::cout << item << " ";
//        }
//        std::cout << std::endl;
//      }
//      std::cout << std::endl;
    }
  }
}
void dyvm::LL1::GetFollower() {

  std::cout << "first" << std::endl;
  for (const auto &[k, v] : first) {
    std::cout << "left " << k << std::endl;
    std::cout << "right ";
    for (const auto &item : v) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  bool changed = true;
  follower[0].insert(EOT);
  while (changed) {
    changed = false;
    for (const auto &[left, right] : productions) {
      for (const auto &item : right) {
        for (int i = 0; i < item.size(); ++i) {
          // If the token is non-terminal, put the first set of the following token into the follower set
          if (IsNonTerminal(item[i])) {
            // Get the follow set of the item
            std::unordered_set<size_t> non_terminal_follow;
            if (auto res = first.find(left); res != first.end()) {
              non_terminal_follow = follower[item[i]];
            }
            bool contain_epsilon = false;
            if (i != (item.size() - 1)) {
              if (IsNonTerminal(item[i + 1])) {
                for (const auto &f : first[item[i + 1]]) {
                  if (f == EPSILON) {
                    contain_epsilon = true;
                  } else if (auto res = non_terminal_follow.find(f); f != EPSILON
                      and res == non_terminal_follow.end()) {
                    changed = true;
                    non_terminal_follow.insert(f);
                  }
                }
              } else if (auto res = non_terminal_follow.find(item[i + 1]); res == non_terminal_follow.end()) {
                changed = true;
                non_terminal_follow.insert(item[i + 1]);
              }
            }
            if ((i == (item.size() - 1)) or contain_epsilon) {
              if (!follower[left].empty()) {
                for (const auto &left_follower : follower[left]) {
                  if (auto res = non_terminal_follow.find(left_follower); res == non_terminal_follow.end()) {
                    changed = true;
                    non_terminal_follow.insert(left_follower);
                  }
                }
              }
            }
            follower[item[i]] = non_terminal_follow;
          }
        }
      }
//      std::cout << "follower" << std::endl;
//      for (const auto &[k, v] : follower) {
//        std::cout << "left " << k << std::endl;
//        std::cout << "right ";
//        for (const auto &item : v) {
//          std::cout << item << " ";
//        }
//        std::cout << std::endl;
//      }
//      std::cout << std::endl;
    }
  }
}
void dyvm::LL1::BuildAnalysisTable() {
  for (const auto &[left, right] : productions) {
    for (const auto &item : right) {
      if (item[0] != EPSILON) {
        if (IsNonTerminal(item[0])) {
          for (const auto &f : first[item[0]]) {
            select[left][f] = std::make_pair(left, item);
          }
        } else {
          select[left][item[0]] = std::make_pair(left, item);
        }
      } else {
        for (const auto &f : follower[left]) {
          select[left][f] = std::make_pair(left, item);
        }
      }
    }
  }

  for (const auto &nt : select) {
    std::cout << "nt: " << nt.first << std::endl;
    for (const auto &t : nt.second) {
      std::cout << " t: " << t.first;
      std::cout << " production: " << t.second.first << " -> ";
      for (const auto &production : t.second.second) {
        std::cout << production << " ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
}

// (1 + 2) * 3 + 4
// (number + number) * number + number
// 8 10 2 10 9 6 10 2 10
void dyvm::LL1::Analyze() {
  std::vector<size_t> tokens{8, 10, 2, 10, 9, 6, 10, 2, 10};
  std::deque<size_t> analysis_stack;
  analysis_stack.push_back(EOT);
  analysis_stack.push_front(0);
  int p = 0;
  while (!analysis_stack.empty()) {
    if (analysis_stack.front() == EPSILON) {
      analysis_stack.pop_front();
    }
    if (analysis_stack.front() != tokens[p]) {
      auto production = select[analysis_stack.front()][tokens[p]];
      auto right = production.second;
      analysis_stack.pop_front();
      std::reverse(right.begin(), right.end());
      for (const auto &item : right) {
        analysis_stack.push_front(item);
      }
    } else if (analysis_stack.front() == tokens[p]) {
      analysis_stack.pop_front();
      p++;
    }
  }
  if (p == tokens.size()) {
    std::cout << "success" << std::endl;
  }
}

std::string dyvm::LL1::FindKey(size_t value) {
  for (const auto &item : materialization) {
    if (item.second == value) {
      return item.first;
    }
  }
  return {};
}

bool dyvm::Parser::IsTerminal(size_t id) {
  return terminals.find(id) != terminals.end();
}

bool dyvm::Parser::IsNonTerminal(size_t id) {
  return non_terminals.find(id) != non_terminals.end();
}

void dyvm::LR0::Preprocess() {
  // Add another start token S_prime to ensure the ubiquity
  std::string s_prime = "S'";
  auto s_prime_id = tokens.size();
  tokens.insert(s_prime);
  materialization.insert(std::make_pair(s_prime, s_prime_id));
  non_terminals.insert(s_prime_id);
  std::vector<size_t> right_items = {0};
  std::vector<std::vector<size_t>> right;
  right.push_back(right_items);
  productions.insert(std::make_pair(s_prime_id, right));
}

void dyvm::LR0::GetDFA() {
  auto s_prime_id = tokens.size() - 1;
  std::stack<state_> state_stack;
  std::vector<state_> states;
  auto start_item = item_(s_prime_id, productions[s_prime_id][0], 0);
  auto start_items = GetEnclosure(start_item);
  state_ state = state_(states.size(), false);
  state.items_ = start_items;
  state_stack.push(state);

  while (!state_stack.empty()) {
    auto curr_dfa_state = state_stack.top();
    state_stack.pop();
    curr_dfa_state.id_ = states.size();

    std::cout << "curr state: " << "state id " << curr_dfa_state.id_ << std::endl;
    for (const auto &item : curr_dfa_state.items_) {
      std::cout << item.left_ << "->";
      for (const auto &right : item.right_) {
        std::cout << right << " ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;

    // For each item, if it is not reduced item, add a transition for the state
    // 1. Get the enclosure of the next item
    // 2. Insert into transitions
    // 3. Push next states into the stack
    // If it is a reduced item, continue
    if (!curr_dfa_state.is_reduced_) {
      for (const auto &item : curr_dfa_state.items_) {
        item_ next_item = item_(item.left_, item.right_, item.dot_ + 1);
        auto next_items = GetEnclosure(next_item);
        auto curr_states = states;
        curr_states.push_back(curr_dfa_state);
        auto s = GetStateFromDFA(curr_states, next_items);
        std::cout << "s.id = " << s.id_ << std::endl;
        if (s.id_ != -1) {
          curr_dfa_state.AddTransition(item.right_[item.dot_], s);

          std::cout << "add state: " << " s: " << item.right_[item.dot_] << std::endl;
          for (const auto &i : s.items_) {
            std::cout << i.left_ << "->";
            for (const auto &right : i.right_) {
              std::cout << right << " ";
            }
            std::cout << std::endl;
          }
          std::cout << std::endl;

        } else {
          state_ next_state = state_();
          next_state.is_reduced_ = next_items.size() == 1;
          next_state.items_ = next_items;
          curr_dfa_state.AddTransition(item.right_[item.dot_], next_state);
          state_stack.push(next_state);

          std::cout << "add state: " << " s: " << item.right_[item.dot_] << std::endl;
          for (const auto &i : next_state.items_) {
            std::cout << i.left_ << "->";
            for (const auto &right : i.right_) {
              std::cout << right << " ";
            }
            std::cout << std::endl;
          }
          std::cout << std::endl;
        }
      }
    }

    states.push_back(curr_dfa_state);
  }
  // Pop the state at the top of the stack
  dfa_.states = states;
}

void dyvm::LR0::BuildAnalysisTable() {
  std::vector<size_t> visited = {0};
  std::stack<state_> state_stack;
  state_stack.push(dfa_.states[start]);

  std::map<size_t, std::map<size_t, AnalysisTable_::Operation_>> action_table;  // action表
  std::map<size_t, std::map<size_t, AnalysisTable_::Operation_>> goto_table;

  for (const auto &curr_dfa_state : dfa_.states) {

    std::cout << "current state: " << curr_dfa_state.id_ << std::endl;
    std::cout << "is reduced: " << curr_dfa_state.is_reduced_ << std::endl;

    if (curr_dfa_state.is_reduced_) {
      AnalysisTable_::Operation_ op = AnalysisTable_::Operation_(AnalysisTable_::REDUCE, -1);
      op.left_ = curr_dfa_state.items_[0].left_;
      op.right_ = curr_dfa_state.items_[0].right_;

      if (op.right_[0] == 0) {
        op.action_ = AnalysisTable_::ACCEPT;
        std::cout << "ACCEPT " << std::endl;
      } else {
        std::cout << "REDUCE " << std::endl;
      }
      std::map<size_t, AnalysisTable_::Operation_> temp;
      for (const auto &t : terminals) {
        temp.insert(std::make_pair(t, op));
      }
      temp.insert(std::make_pair(EOT, op));
      if (auto res = action_table.find(curr_dfa_state.id_); res == action_table.end()) {
        action_table.insert(std::make_pair(curr_dfa_state.id_, temp));
      } else {
        for (const auto &t : terminals) {
          action_table[curr_dfa_state.id_].insert(std::make_pair(t, op));
        }
        action_table[curr_dfa_state.id_].insert(std::make_pair(EOT, op));
      }

    } else {
      for (const auto &transition : curr_dfa_state.transitions_) {
        state_ s = GetStateFromDFA(dfa_.states, transition.second.items_);

        if (IsTerminal(transition.first)) {
          std::map<size_t, AnalysisTable_::Operation_> temp;
          temp.insert(std::make_pair(transition.first,
                                     AnalysisTable_::Operation_(AnalysisTable_::SHIFT, s.id_)));
          if (auto res = action_table.find(curr_dfa_state.id_); res == action_table.end()) {
            action_table.insert(std::make_pair(curr_dfa_state.id_, temp));
          } else {
            action_table[curr_dfa_state.id_].insert(std::make_pair(transition.first,
                                                                   AnalysisTable_::Operation_(AnalysisTable_::SHIFT,
                                                                                              s.id_)));
          }
          std::cout << "SHIFT " << s.id_ << std::endl;
        } else if (IsNonTerminal(transition.first)) {
          std::map<size_t, AnalysisTable_::Operation_> temp;

          temp.insert(std::make_pair(transition.first,
                                     AnalysisTable_::Operation_(AnalysisTable_::GOTO, s.id_)));

          if (auto res = goto_table.find(curr_dfa_state.id_); res == goto_table.end()) {
            goto_table.insert(std::make_pair(curr_dfa_state.id_, temp));
          } else {
            goto_table[curr_dfa_state.id_].insert(std::make_pair(transition.first,
                                                                 AnalysisTable_::Operation_(AnalysisTable_::GOTO,
                                                                                            s.id_)));
          }
          std::cout << "GOTO " << s.id_ << std::endl;
        }
      }
    }
  }
  analysis_table_.action_table_ = action_table;
  analysis_table_.goto_table_ = goto_table;
}

// e.g. accccd
void dyvm::LR0::Analyze() {
  std::vector<size_t> tokens{1, 4, 4, 4, 4, 5, EOT};
  std::deque<size_t> analysis_stack;
  analysis_stack.push_back(EOT);
  analysis_stack.push_front(0);
  int p = 0;
  while (true) {
    auto it = analysis_stack[0];
    auto op = analysis_table_.action_table_[it][tokens[p]];
    if (op.action_ == AnalysisTable_::SHIFT) {
      std::cout << "SHIFT" << std::endl;
      analysis_stack.push_front(tokens[p]);
      analysis_stack.push_front(op.next);
      p++;
    } else if (op.action_ == AnalysisTable_::REDUCE) {
      std::cout << "REDUCE" << std::endl;
      for (int i = 0; i < 2 * op.right_.size(); ++i) {
        analysis_stack.pop_front();
      }
      auto top = analysis_stack.front();
      analysis_stack.push_front(op.left_);
      auto s1 = analysis_table_.goto_table_[top][op.left_];
      analysis_stack.push_front(s1.next);
    } else if (op.action_ == AnalysisTable_::ACCEPT) {
      std::cout << "success" << std::endl;
      return;
    }

    for (const auto &item : analysis_stack) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }
}

dyvm::LR0::state_ dyvm::LR0::StatesContainItems(const std::vector<state_> &states, const std::vector<item_> &items) {
  for (const auto &state : states) {
    for (const auto &item : state.items_) {
      if (ItemsContainItem(items, item)) {
        return state;
      }
    }
  }
  return {};
}

dyvm::LR0::state_ dyvm::LR0::GetStateFromDFA(const std::vector<state_> &states, const std::vector<item_> &items) {
  for (const auto &state : states) {
    bool res = true;
    for (const auto &item : items) {
      if (!ItemsContainItem(state.items_, item)) {
        res = false;
      }
    }
    if (res) {
      return state;
    }
  }
  return {};
}

bool dyvm::LR0::ItemsContainItem(const std::vector<item_> &items, const item_ &item) {
  for (const auto &i : items) {
    if (i.left_ == item.left_ && i.dot_ == item.dot_ && item_::VectorIsEqual(i.right_, item.right_)) {
      return true;
    }
  }
  return false;
}

void dyvm::SLR1::GetDFA() {
  LR0::GetDFA();
  for (const auto &state : dfa_.states) {
    if (state.items_.size() > 1 and state.items_[0].dot_ > 0) {
      bool conflict_flag = true;
      size_t common_nonterminal = state.items_[0].right_[state.items_[0].dot_ - 1];
      for (const auto &item : state.items_) {
        if (item.dot_ != 0 and item.right_[item.dot_ - 1] != common_nonterminal) {
          conflict_flag = false;
        }
      }
      if (conflict_flag) {
        conflict_items.push_back(state.id_);
      }
    }
  }
}

// TODO Build analysis table
void dyvm::SLR1::BuildAnalysisTable() {
  if (!can_slr1) {
    std::cerr << "Cannot use SLR(1)!" << std::endl;
    return;
  }

  std::vector<size_t> visited = {0};
  std::stack<state_> state_stack;
  state_stack.push(dfa_.states[start]);

  std::map<size_t, std::map<size_t, AnalysisTable_::Operation_>> action_table;  // action表
  std::map<size_t, std::map<size_t, AnalysisTable_::Operation_>> goto_table;

  for (const auto &curr_dfa_state : dfa_.states) {
    std::cout << "current state: " << curr_dfa_state.id_ << std::endl;
    std::cout << "is reduced: " << curr_dfa_state.is_reduced_ << std::endl;
    if (curr_dfa_state.is_reduced_) {
      AnalysisTable_::Operation_ op = AnalysisTable_::Operation_(AnalysisTable_::REDUCE, -1);
      op.left_ = curr_dfa_state.items_[0].left_;
      op.right_ = curr_dfa_state.items_[0].right_;
      if (op.right_[0] == 0) {
        op.action_ = AnalysisTable_::ACCEPT;
        std::cout << "ACCEPT " << std::endl;
      } else {
        std::cout << "REDUCE " << std::endl;
      }
      std::map<size_t, AnalysisTable_::Operation_> temp;
      for (const auto &t : terminals) {
        temp.insert(std::make_pair(t, op));
      }
      temp.insert(std::make_pair(EOT, op));
      if (auto res = action_table.find(curr_dfa_state.id_); res == action_table.end()) {
        action_table.insert(std::make_pair(curr_dfa_state.id_, temp));
      } else {
        // Add to all follow of the left
        for (const auto &t : ll_1_parser.follower[curr_dfa_state.items_[0].left_]) {
          action_table[curr_dfa_state.id_].insert(std::make_pair(t, op));
        }
        action_table[curr_dfa_state.id_].insert(std::make_pair(EOT, op));
      }
    } else {
      // Whether it is conflict item
      if (auto conflict = std::find(conflict_items.begin(), conflict_items.end(), curr_dfa_state.id_); conflict
          == conflict_items.end()) {
        for (const auto &transition : curr_dfa_state.transitions_) {
          state_ s = GetStateFromDFA(dfa_.states, transition.second.items_);
          if (IsTerminal(transition.first)) {
            std::map<size_t, AnalysisTable_::Operation_> temp;
            temp.insert(std::make_pair(transition.first,
                                       AnalysisTable_::Operation_(AnalysisTable_::SHIFT, s.id_)));
            if (auto res = action_table.find(curr_dfa_state.id_); res == action_table.end()) {
              action_table.insert(std::make_pair(curr_dfa_state.id_, temp));
            } else {
              action_table[curr_dfa_state.id_].insert(std::make_pair(transition.first,
                                                                     AnalysisTable_::Operation_(AnalysisTable_::SHIFT,
                                                                                                s.id_)));
            }
            std::cout << "SHIFT " << s.id_ << std::endl;
          } else if (IsNonTerminal(transition.first)) {
            std::map<size_t, AnalysisTable_::Operation_> temp;

            temp.insert(std::make_pair(transition.first,
                                       AnalysisTable_::Operation_(AnalysisTable_::GOTO, s.id_)));

            if (auto res = goto_table.find(curr_dfa_state.id_); res == goto_table.end()) {
              goto_table.insert(std::make_pair(curr_dfa_state.id_, temp));
            } else {
              goto_table[curr_dfa_state.id_].insert(std::make_pair(transition.first,
                                                                   AnalysisTable_::Operation_(AnalysisTable_::GOTO,
                                                                                              s.id_)));
            }
            std::cout << "GOTO " << s.id_ << std::endl;
          }
        }
      } else {
        // Conflict item
        for (const auto &item : curr_dfa_state.items_) {
          if (item.dot_ == item.right_.size()) {
            // Reduce item
            AnalysisTable_::Operation_ op = AnalysisTable_::Operation_(AnalysisTable_::REDUCE, -1);
            op.left_ = item.left_;
            op.right_ = item.right_;
            if (op.right_[0] == 0) {
              op.action_ = AnalysisTable_::ACCEPT;
              std::cout << "ACCEPT " << std::endl;
            } else {
              std::cout << "REDUCE " << std::endl;
            }
            std::map<size_t, AnalysisTable_::Operation_> temp;
            for (const auto &t : terminals) {
              temp.insert(std::make_pair(t, op));
            }
            temp.insert(std::make_pair(EOT, op));
            if (auto res = action_table.find(curr_dfa_state.id_); res == action_table.end()) {
              action_table.insert(std::make_pair(curr_dfa_state.id_, temp));
            } else {
              // Add to all follow of the left
              for (const auto &t : ll_1_parser.follower[item.left_]) {
                action_table[curr_dfa_state.id_].insert(std::make_pair(t, op));
              }
              action_table[curr_dfa_state.id_].insert(std::make_pair(EOT, op));
            }
          } else {
            // Shift item
            for (const auto &transition : curr_dfa_state.transitions_) {
              state_ s = GetStateFromDFA(dfa_.states, transition.second.items_);
              if (IsTerminal(transition.first)) {
                std::map<size_t, AnalysisTable_::Operation_> temp;
                temp.insert(std::make_pair(transition.first,
                                           AnalysisTable_::Operation_(AnalysisTable_::SHIFT, s.id_)));
                if (auto res = action_table.find(curr_dfa_state.id_); res == action_table.end()) {
                  action_table.insert(std::make_pair(curr_dfa_state.id_, temp));
                } else {
                  action_table[curr_dfa_state.id_].insert(std::make_pair(transition.first,
                                                                         AnalysisTable_::Operation_(AnalysisTable_::SHIFT,
                                                                                                    s.id_)));
                }
                std::cout << "SHIFT " << s.id_ << std::endl;
              } else if (IsNonTerminal(transition.first)) {
                std::map<size_t, AnalysisTable_::Operation_> temp;

                temp.insert(std::make_pair(transition.first,
                                           AnalysisTable_::Operation_(AnalysisTable_::GOTO, s.id_)));

                if (auto res = goto_table.find(curr_dfa_state.id_); res == goto_table.end()) {
                  goto_table.insert(std::make_pair(curr_dfa_state.id_, temp));
                } else {
                  goto_table[curr_dfa_state.id_].insert(std::make_pair(transition.first,
                                                                       AnalysisTable_::Operation_(AnalysisTable_::GOTO,
                                                                                                  s.id_)));
                }
                std::cout << "GOTO " << s.id_ << std::endl;
              }
            }
          }
        }
      }
    }
  }
  analysis_table_.action_table_ = action_table;
  analysis_table_.goto_table_ = goto_table;
}

void dyvm::SLR1::JudgeCanSLR1() {
  ll_1_parser.Preprocess();
  ll_1_parser.GetFirst();
  ll_1_parser.GetFollower();

  for (const auto &id : conflict_items) {
    std::unordered_set<size_t> intersection;
    auto state = dfa_.states[id];
    for (const auto &item : state.items_) {
      if (item.dot_ == item.right_.size()) {
        auto set1 = ll_1_parser.follower[item.left_];
        if (intersection.empty()) {
          intersection.insert(set1.begin(), set1.end());
        } else {
          auto inter = GetIntersection(intersection, set1);
          intersection.insert(inter.begin(), inter.end());
        }
      } else {
        std::unordered_set<size_t> set1{item.right_[item.dot_]};
        if (intersection.empty()) {
          intersection.insert(set1.begin(), set1.end());
        } else {
          auto inter = GetIntersection(intersection, set1);
          intersection.insert(inter.begin(), inter.end());
        }
      }
    }
    if (!intersection.empty()) {
      can_slr1 = false;
    }
  }
}

std::unordered_set<size_t> dyvm::SLR1::GetIntersection(std::unordered_set<size_t> set1,
                                                       std::unordered_set<size_t> set2) {
  std::set<size_t> orderedSet1(set1.begin(), set1.end());
  std::set<size_t> orderedSet2(set2.begin(), set2.end());

  std::set<size_t> intersection;

  std::set_intersection(orderedSet1.begin(), orderedSet1.end(),
                        orderedSet2.begin(), orderedSet2.end(),
                        std::inserter(intersection, intersection.begin()));

  set1.clear();
  set1.insert(intersection.begin(), intersection.end());

  return std::unordered_set<size_t>{intersection.begin(), intersection.end()};
}



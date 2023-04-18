//
// Created by Dysprosium on 2023/4/6.
//
#include "parser.hpp"

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
          for (int j = 0; j < i.size(); ++j) {
            bool no_epsilon = true;
            if (IsTerminal(i[j]) or i[j] == EPSILON) {
              if (auto res = non_terminal_first.find(i[j]); res
                  == non_terminal_first.end()) {
                if (i[j] == EPSILON) {
                  no_epsilon = false;
                }
                changed = true;
                non_terminal_first.insert(i[j]);
              }
            }
            if (auto res = first.find(i[j]); res != first.end()) {
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

bool dyvm::LL1::IsTerminal(size_t id) {
  return terminals.find(id) != terminals.end();
}

bool dyvm::LL1::IsNonTerminal(size_t id) {
  return non_terminals.find(id) != non_terminals.end();
}


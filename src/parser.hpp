//
// Created by Dysprosium on 2023/4/6.
//

#ifndef DYVM_SRC_PARSER_HPP_
#define DYVM_SRC_PARSER_HPP_

#include <utility>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

namespace dyvm {

class Parser {
 public:
  explicit Parser(std::string s) {
    syntax_rules_path = std::move(s);
    std::ifstream f(syntax_rules_path);
    nlohmann::json data = nlohmann::json::parse(f);
    std::map<std::string, std::vector<std::vector<std::string>>> temp_productions;
    for (const auto &rule : data["rules"]) {
      size_t non_terminal_id = tokens.size();
      materialization.insert(std::make_pair(rule["name"], non_terminal_id));
      non_terminals.insert(non_terminal_id);
      tokens.insert(rule["name"]);
      for (const auto &production : rule["productions"]) {
        std::vector<std::string> right;
        for (const auto &symbol : production["symbols"]) {
          right.push_back(symbol["value"]);
          if (symbol["type"] == "terminal") {
            size_t terminal_id = tokens.size();
            materialization.insert(std::make_pair(symbol["value"], terminal_id));
            terminals.insert(terminal_id);
            tokens.insert(symbol["value"]);
          }
        }
        temp_productions[rule["name"]].push_back(right);
      }
    }

    materialization.insert(std::make_pair(std::string(), -1));

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

    for (const auto &[nt, symbols] : temp_productions) {
      for (const auto &symbol : symbols) {
        std::vector<size_t> right;
        for (const auto &item : symbol) {
          right.push_back(materialization[item]);
        }
        productions[materialization[nt]].push_back(right);
      }
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
 public:
  bool IsTerminal(size_t id);
  bool IsNonTerminal(size_t id);

  virtual void Preprocess() = 0;
  virtual void BuildAnalysisTable() = 0;
  virtual void Analyze() = 0;

 public:
  enum {
    EPSILON = 18446744073709551615,
    EOT = 18446744073709551614
  };
  size_t start = 0;
  std::unordered_set<std::string> tokens;
  std::unordered_set<size_t> terminals;
  std::unordered_set<size_t> non_terminals;
  std::unordered_map<std::string, size_t> materialization;
  std::map<size_t, std::vector<std::vector<size_t>>> productions;
 public:
  std::string syntax_rules_path = "../test/lr0.json";

};

class LL1 : public Parser {
 public:
  LL1(std::string s) : Parser(s) {
  };

 public:
  void Preprocess() override;
  void GetFirst();
  void GetFollower();
  void BuildAnalysisTable() override;
  void Analyze() override;

 public:
  std::unordered_map<size_t, std::unordered_set<size_t>> first;
  std::unordered_map<size_t, std::unordered_set<size_t>> follower;
  std::unordered_map<size_t, std::unordered_map<size_t, std::pair<size_t, std::vector<size_t>>>> select;

 private:
  std::string FindKey(size_t value);

};

class LR0 : public Parser {
 public:
  LR0(std::string s) : Parser(s) {
  }

 public:
  class item_ {
   public:
    size_t left_;
    std::vector<size_t> right_;
    size_t dot_;

    item_(size_t left, const std::vector<size_t> &right, size_t point) : left_(left), right_(right), dot_(point) {}

    static bool VectorIsEqual(std::vector<size_t> v1, std::vector<size_t> v2) {
      if (v1.size() != v2.size()) {
        return false;
      }
      for (int i = 0; i < v1.size(); i++) {
        if (v1[i] != v2[i]) {
          return false;
        }
      }
      return true;
    }

    bool operator==(const item_ &rhs) const {
      return left_ == rhs.left_ &&
          VectorIsEqual(right_, rhs.right_)
          &&
              dot_ == rhs.dot_;
    }

    bool operator!=(const item_ &rhs) const {
      return !(rhs == *this);
    }

    static bool ItemsContain(const std::vector<item_> &items, const item_ &target) {
      for (const auto &item : items)
        if (item == target)
          return true;
      return false;
    }

  };

  class state_ {
   public:
    state_(size_t id, bool is_reduced) : id_(id), is_reduced_(is_reduced) {}
    explicit state_(size_t id) : id_(id) {}
    state_() = default;
    size_t id_ = -1;
    bool is_reduced_{};
    std::map<size_t, state_> transitions_;
    std::vector<item_> items_;

   public:
    void AddTransition(const size_t s, const state_ &state) {
      transitions_.insert(std::make_pair(s, state));
    };

  };

  class DFA_ {
   public:
    size_t start = 0;
    std::vector<state_> states;
  };

  class AnalysisTable_ {
   public:

    enum action_ {
      SHIFT,
      REDUCE,
      ACCEPT,
      GOTO
    };

    class Operation_ {

     public:
      Operation_() {}
      Operation_(action_ action, size_t next) : action_(action), next(next) {}
     public:
      size_t left_{};
      std::vector<size_t> right_;
      action_ action_;
      size_t next;
    };

    std::map<size_t, std::map<size_t, AnalysisTable_::Operation_>> action_table_;  // action表
    std::map<size_t, std::map<size_t, AnalysisTable_::Operation_>> goto_table_;    // goto表
  };

 public:
  DFA_ dfa_;
  AnalysisTable_ analysis_table_;

 public:
  void Preprocess() override;
  virtual void GetDFA();
  void BuildAnalysisTable() override;
  void Analyze() override;

 protected:
  std::vector<item_> GetEnclosure(item_ &items) {
    std::vector<item_> res = {items};
    std::vector<item_> res1 = {items};
    bool changed = false;
    while (!changed) {
      changed = false;
      for (const auto &item : res) {
        if (item.dot_ == item.right_.size()) {
          return res1;
        }
        if (IsNonTerminal(item.right_[item.dot_])) {
          for (const auto &prod : productions[item.right_[item.dot_]]) {
            // Construct new item
            auto left = item.right_[item.dot_];
            auto right = prod;
            item_ new_item = item_(left, right, 0);
            if (!item_::ItemsContain(res1, new_item)) {
              res1.push_back(new_item);
              changed = true;
            }
          }
        }
      }
    }
    return res1;
  };

  static state_ StatesContainItems(const std::vector<state_> &states, const std::vector<item_> &items);
  static bool ItemsContainItem(const std::vector<item_> &items, const item_ &item);
  state_ GetStateFromDFA(const std::vector<state_> &states, const std::vector<item_> &items);
};

class SLR1: public LR0 {
 public:
  explicit SLR1(const std::string &s) : LR0(s), ll_1_parser(s) {
  };
 public:
  void GetDFA() override;
  void JudgeCanSLR1();
  void BuildAnalysisTable() override;


 public:
  bool can_slr1 = true;
  std::vector<size_t> conflict_items;
  LL1 ll_1_parser;

 private:
  std::unordered_set<size_t> GetIntersection(std::unordered_set<size_t> set1, std::unordered_set<size_t> set2);

};


}
#endif //DYVM_SRC_PARSER_HPP_

//
// Created by Dysprosium on 2023/4/6.
//

#ifndef DYVM_SRC_PARSER_HPP_
#define DYVM_SRC_PARSER_HPP_

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
  Parser() {
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
  size_t start = 0;
  std::unordered_set<std::string> tokens;
  std::unordered_set<size_t> terminals;
  std::unordered_set<size_t> non_terminals;
  std::unordered_map<std::string, size_t> materialization;
  std::map<size_t, std::vector<std::vector<size_t>>> productions;
 public:
  std::string syntax_rules_path = "../test/syntax_rules.json";

};

class LL1 : Parser {
 public:
  LL1() : Parser() {
  };

 public:
  void Preprocess();
  void GetFirst();
  void GetFollower();
  void BuildAnalysisTable();
  void Analyze();

 public:
  std::unordered_map<size_t, std::unordered_set<size_t>> first;
  std::unordered_map<size_t, std::unordered_set<size_t>> follower;
  std::unordered_map<size_t, std::unordered_map<size_t, std::pair<size_t, std::vector<size_t>>>> select;


 private:
  enum {
    EPSILON = 18446744073709551615,
    EOT = 18446744073709551614
  };

 private:
  std::string FindKey(size_t value);
  bool IsTerminal(size_t id);
  bool IsNonTerminal(size_t id);
};

}
#endif //DYVM_SRC_PARSER_HPP_

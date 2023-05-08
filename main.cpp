#include <iostream>
#include <stack>
#include "src/nfa.hpp"
#include "src/file_stream.hpp"
#include "src/dfa.hpp"
#include "src/lex.hpp"
#include "src/parser.hpp"

int main() {
//  std::string path = "../test/regex_input.txt";
//  dyvm::FileStream file(path);
//  file.Open();
//  auto regex = file.Read();
//  dyvm::NFA nfa;
//  auto result = nfa.RegexToNFA(regex);
//  result.Print();
//  dyvm::DFA dfa(result);
//  auto res = dfa.EpsilonEnclosure(result.start);
//
//  dfa.ConvertNFAToDFA();
//  std::cout << "start_ids" << ' ';
//  for (const auto &item : dfa.start_ids) {
//    std::cout << item << ' ';
//  }
//  std::cout << std::endl;
//  for (const auto &d : dfa.states) {
//    std::cout << d.id << ' ' << &d << ' ' << "accepted " << d.accepted << ' ';
//    for (const auto &t : d.transitions) {
//      std::cout << t.first << ' ';
//      for (const auto &item : t.second) {
//        std::cout << item.id << ' ' << &item;
//      }
//    }
//    std::cout << std::endl;
//  }
//  dfa.MinimizeDFA();
//  std::cout << "After minimize:" << std::endl;
//  std::cout << "start_ids" << ' ';
//  for (const auto &item : dfa.start_ids) {
//    std::cout << item << ' ';
//  }
//  std::cout << std::endl;
//  for (const auto &d : dfa.states) {
//    std::cout << d.id << ' ' << &d << ' ' << "accepted " << d.accepted << ' ';
//    for (const auto &t : d.transitions) {
//      std::cout << t.first << ' ';
//      for (const auto &item : t.second) {
//        std::cout << item.id << ' ' << &item << ' ';
//      }
//    }
//    std::cout << std::endl;
//  }

//  dyvm::Lex lex = dyvm::Lex();
//  lex.Analyze();

//  dyvm::LL1 parser = dyvm::LL1("../test/syntax_rules.json");
//  parser.Preprocess();
//  parser.GetFirst();
//  parser.GetFollower();
//  parser.BuildAnalysisTable();
//  parser.Analyze();

  dyvm::LR0 parser = dyvm::LR0("../test/lr0.json");
  parser.Preprocess();
  parser.GetDFA();
  parser.BuildAnalysisTable();
}

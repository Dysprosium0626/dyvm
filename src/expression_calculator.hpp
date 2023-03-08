//
// Created by Dysprosium on 2023/3/5.
//

#ifndef DYVM_SRC_EXPRESSION_CALCULATOR_HPP_
#define DYVM_SRC_EXPRESSION_CALCULATOR_HPP_
#include <string>
#include <unordered_map>

namespace dyvm {

class ExpressionCalculator {
 public:
  // Create a new ExpressionCalculator object with given expression in the style of infix expression.
  ExpressionCalculator(std::string expression);

  // Calculate the result of infix expression.
  int calculate();

 private:
  // Infix expression
  std::string expression_;

  // The result of the infix expression
  int result_;

  // A hashmap to store the priority of operators
  std::unordered_map<char, int> priority_;

};
} // dyvm


#endif //DYVM_SRC_EXPRESSION_CALCULATOR_HPP_

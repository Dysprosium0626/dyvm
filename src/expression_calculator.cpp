//
// Created by Dysprosium on 2023/3/5.
//

#include "expression_calculator.hpp"
#include <stack>
#include <vector>

namespace dyvm {

// Calculate the result of two operate numbers with certain operator.
int CalculateWithOperator(int x, int y, char op) {
  int ret = 0;
  if (op == '+') ret = x + y;
  else if (op == '-') ret = x - y;
  else if (op == '*') ret = x * y;
  else if (op == '/') ret = x / y;
  return ret;
}

// Init the ExpressionCalculator object with infix expression and a hashmap stored with priority list of operators.
ExpressionCalculator::ExpressionCalculator(std::string expression) : expression_(expression) {
  priority_.insert(std::make_pair('+', 1));
  priority_.insert(std::make_pair('-', 1));
  priority_.insert(std::make_pair('*', 2));
  priority_.insert(std::make_pair('/', 2));
  calculate();
}

// When we traverse the expression and the operator is less prior to what is previously stored in stack,
// we should pop the operator stack and do calculation with the last two numbers in the number vector and
// push the result into the operator stack.
void PopCalculate(std::stack<char> &s, std::vector<int> &num) {//弹栈，做运算，再存栈
  char op = s.top();// Pop up the operator stack
  s.pop();
  // Pop up the last two number in the vector and conduct calculation.
  int y = num.back();
  num.pop_back();
  int x = num.back();
  num.pop_back();
  num.push_back(CalculateWithOperator(x, y, op));// Push the result into the number vector.
}

int ExpressionCalculator::calculate() {
  std::stack<char> s;
  std::vector<int> num;
  bool flag = false; // The flag that indicate whether the number is the first number.
  for (int i = 0; i < expression_.length(); ++i) {
    if (expression_[i] == '(') {
      if (flag == true) {
        flag = false;
      }
      s.push(expression_[i]); // Push the left into the operator stack every time we meet it.
    } else if (expression_[i] == ')') {
      if (flag == true) {
        flag = false;
      }
      if (s.empty()) {
        break;
      } else {
        // Pop up and conduct calculation recursively once we get the right till the top of stack is the left.
        while (s.top() != '(') {
          PopCalculate(s, num);
          if (s.empty()) {
            break;
          }
        }
        s.pop();
      }
    } else if (expression_[i] == '+' || expression_[i] == '-' || expression_[i] == '*' || expression_[i] == '/') {
      if (flag == true) {
        flag = false;
      }
      // Compare the priority of operators according to the hashmap.
      while (!s.empty() && s.top() != '('
          && priority_.find(s.top())->second >= priority_.find(expression_[i])->second) {
        PopCalculate(s, num);
      }
      s.push(expression_[i]);

    } else {
      // Push the number into vector every time we meet it.
      int tmp = expression_[i] - '0';
      if (flag) {
        *(num.end() - 1) = *(num.end() - 1) * 10 + tmp;
      } else {
        num.push_back(tmp);
        flag = true;
      }
    }
  }
  // After a thorough traverse we recursively do calculation with
  // the top operator in stack with last two numbers in vector until
  // the stack is empty.
  while (!s.empty()) {
    PopCalculate(s, num);
  }
  // Assign the result with the last number in vector.
  int result = num.back();
  return result;
}

} // dyvm
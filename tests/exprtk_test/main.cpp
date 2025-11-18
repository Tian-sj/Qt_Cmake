#include <iostream>
#include <vector>
#include "exprtk.hpp"

int main() {
  using T = double;

  std::vector<T> vin{1,2,3,4};
  std::vector<T> vout(vin.size());   // 结果向量，大小要先准备好

  exprtk::symbol_table<T> st;
  st.add_vector("vin",  vin);        // 暴露输入向量
  st.add_vector("vout", vout);       // 暴露输出向量
  st.add_constants();

  exprtk::expression<T> expr;
  expr.register_symbol_table(st);

  exprtk::parser<T> parser;

  // 逐元素：vout[i] = 2 * vin[i] + 1
  if (!parser.compile("vout := 2 * vin + 1", expr)) {
    std::cerr << parser.error() << "\n";
    return 1;
  }

  expr.value(); // 执行；结果写进 vout

  for (auto x : vout) std::cout << x << " "; // 3 5 7 9
  std::cout << "\n";

  // 如果你想要标量：sum(2*vin+1) 会做完逐元素再求和
  parser.compile("sum(2 * vin + 1)", expr);
  std::cout << "sum = " << expr.value() << "\n"; // 3+5+7+9=24
}
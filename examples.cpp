#include "CalcString.hpp"
#include <iostream>

int main() {
  using namespace calcStringNS;
  using namespace std;
  cout <<"(2*(5+3)+4)^2 = "<< calcString<decltype("(2*(5+3)+4)^2"_tstr)> << endl;
  static_assert(calcString<decltype("(2*(5+3)+4)^2"_tstr)> == 400);
  using context = userContext<>
  ::add<decltype("a=2"_tstr)>
  ::add<decltype("f(x)=2*x+x"_tstr)>
  ::add<decltype("g(x)=f(x)+f(1)"_tstr)>;
  cout<<"with the following declerations in context: "<<endl;
  cout<<"a=2"<<endl;
  cout<<"f(x)=2*x+x"<<endl;
  cout<<"g(x)=f(x)+f(1)"<<endl;
  cout<<"the expressions evaluate as:"<<endl;
  cout <<"a = "<< calcString<decltype("a"_tstr), context> << endl;
  static_assert(calcString<decltype("a"_tstr), context> == 2);
  cout <<"f(1) = "<< calcString<decltype("f(1)"_tstr), context> << endl;
  static_assert(calcString<decltype("f(1)"_tstr), context> == 3);
  cout <<"f(a)+1 = "<<calcString<decltype("f(a)+1"_tstr), context> << endl;
  static_assert(calcString<decltype("f(a)+1"_tstr), context> == 7);
  cout <<"g(a) = "<< calcString<decltype("g(a)"_tstr), context> << endl;
  static_assert(calcString<decltype("g(a)"_tstr), context> == 9);
  cout <<"g(f(g(a))) = "<< calcString<decltype("g(f(g(a)))"_tstr), context> << endl;
  static_assert(calcString<decltype("g(f(g(a)))"_tstr), context> == 84);

  using expr = decltype("f(a)+g(a)"_tstr); // the expression that will be evaluated.

  constexpr int exprValue = calcString<expr,context>;

  static_assert(exprValue == 15);
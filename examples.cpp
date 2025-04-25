#inlcude<CalcString.hpp>

int main() {
  using namespace calcStringNS;
  using namespace std;
  cout << calcString<decltype("(2*(5+3)+4)^2"_tstr)> << endl;
  static_assert(calcString<decltype("(2*(5+3)+4)^2"_tstr)> == 400);
  using context = userContext<>
  ::add<decltype("a=2"_tstr)>
  ::add<decltype("f(x)=2*x+x"_tstr)>
  ::add<decltype("g(x)=f(x)+f(1)"_tstr)>;

  cout << calcString<decltype("a"_tstr), context> << endl;
  static_assert(calcString<decltype("a"_tstr), context> == 2);
  cout << calcString<decltype("f(1)"_tstr), context> << endl;
  static_assert(calcString<decltype("f(1)"_tstr), context> == 3);
  cout << calcString<decltype("f(a)+1"_tstr), context> << endl;
  static_assert(calcString<decltype("f(a)+1"_tstr), context> == 7);
  cout << calcString<decltype("g(a)"_tstr), context> << endl;
  static_assert(calcString<decltype("g(a)"_tstr), context> == 9);
}
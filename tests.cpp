#inlcude<calcString.hpp>

namespace{
using namespace calcStringNS;
struct TESTS {
  
  static_assert(calcString<decltype("-1"_tstr)> == -1);
  static_assert(calcString<decltype("(-1)"_tstr)> == -1);
  static_assert(calcString<decltype("-2354"_tstr)> == -2354);
  static_assert(calcString<decltype("23451"_tstr)> == 23451);
  static_assert(calcString<decltype("5+7"_tstr)> == 12);
  static_assert(calcString<decltype("(-1)+5"_tstr)> == 4);
  static_assert(calcString<decltype("7+(-4)"_tstr)> == 3);
  static_assert(calcString<decltype("5*6"_tstr)> == 30);
  static_assert(calcString<decltype("3*(-7)"_tstr)> == -21);
  static_assert(calcString<decltype("51/10"_tstr)> == 5);
  static_assert(calcString<decltype("(-56)/11"_tstr)> == -5);
  static_assert(calcString<decltype("3^3"_tstr)> == 27);
  static_assert(calcString<decltype("(-3)^3"_tstr)> == -27);
  static_assert(calcString<decltype("(-3)^2"_tstr)> == 9);
  static_assert(calcString<decltype(""_tstr)> == 0);

  static_assert(calcString<decltype("(2*(5+3)+4)^2"_tstr)> == 400);
  static_assert(calcString<decltype("((2*(5+3)+4)^2)"_tstr)> == 400);
};

struct TEST2 {
  struct a {
    using context = userContext<>
    ::add<decltype("f(x)=x^2+x+1"_tstr)>
    ::add<decltype("g(x)=x^3+(-2*x)+(-2)"_tstr)>
    ::add<decltype("h(x)=g(f(x))"_tstr)>;
    
    static_assert(calcString<decltype("f(0)"_tstr), context> == 1);
    static_assert(calcString<decltype("f(1)"_tstr), context> == 3);
    static_assert(calcString<decltype("f(-1)"_tstr), context> == 1);
    static_assert(calcString<decltype("f(-2)"_tstr), context> == 3);
    static_assert(calcString<decltype("f(-3)"_tstr), context> == 7);
    static_assert(calcString<decltype("f(2)"_tstr), context> == 7);
    static_assert(calcString<decltype("f(3)"_tstr), context> == 13);

    static_assert(calcString<decltype("g(0)"_tstr), context> == -2);
    static_assert(calcString<decltype("g(1)"_tstr), context> == -3);
    static_assert(calcString<decltype("g(2)"_tstr), context> == 2);
    static_assert(calcString<decltype("g(3)"_tstr), context> == 19);
    static_assert(calcString<decltype("g(-1)"_tstr), context> == -1);
    static_assert(calcString<decltype("g(-2)"_tstr), context> == -6);
    static_assert(calcString<decltype("g(-3)"_tstr), context> == -23);

    static_assert(calcString<decltype("h(0)"_tstr), context> == -3);
    static_assert(calcString<decltype("h(1)"_tstr), context> == 19);
    static_assert(calcString<decltype("h(2)"_tstr), context> == 327);
    static_assert(calcString<decltype("h(3)"_tstr), context> == 2169);
    static_assert(calcString<decltype("h(-1)"_tstr), context> == -3);
    static_assert(calcString<decltype("h(-2)"_tstr), context> == 19);
    static_assert(calcString<decltype("h(-3)"_tstr), context> == 327);
  };
};
}

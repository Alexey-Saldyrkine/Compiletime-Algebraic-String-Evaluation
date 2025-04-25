#include <charconv>

namespace calcStringNS{
template <char... chars>
using tstring = std::integer_sequence<char, chars...>;

template <typename T, T... chars>
constexpr tstring<chars...> operator""_tstr() {
  return {};
}
namespace detail{
template <typename T>
struct type_tag {
  using type = T;
};

template <typename K, typename V>
struct typePair {
  using first_type = K;
  using second_type = V;
};

template <typename Pair>
struct element {
  static auto value(type_tag<typename Pair::first_type>)
      -> type_tag<typename Pair::second_type>;
};

template <typename... elems>
struct type_map : element<elems>... {
  using element<elems>::value...;

  template <typename K>
  using find = typename decltype(type_map::value(type_tag<K>{}))::type;

  template <typename K, typename V>
  using add = type_map<typePair<K, V>, elems...>;

  template <typename T>
  static constexpr bool hasType = (std::is_same_v<T, elems> || ...);
};

template <typename T, T, typename>
struct prependIntSeq;

template <typename T, T chr, T... c>
struct prependIntSeq<T, chr, std::integer_sequence<T, c...>> {
  using type = std::integer_sequence<T, chr, c...>;
};

template <size_t, size_t, size_t, typename>
struct subSeqImpl;

template <size_t i, size_t s, size_t e, char cur, char... c>
struct subSeqImpl<i, s, e, std::integer_sequence<char, cur, c...>> {
  using type =
      typename subSeqImpl<i + 1, s, e, std::integer_sequence<char, c...>>::type;
};

template <size_t i, size_t e, char cur, char... c>
struct subSeqImpl<i, i, e, std::integer_sequence<char, cur, c...>> {
  using type = typename prependIntSeq<
      char, cur,
      typename subSeqImpl<i + 1, i + 1, e,
                          std::integer_sequence<char, c...>>::type>::type;
};

template <size_t i, char cur, char... c>
struct subSeqImpl<i, i, i, std::integer_sequence<char, cur, c...>> {
  using type = std::integer_sequence<char, cur>;
};

template <size_t, size_t, typename>
struct subSeq;

template <size_t i, size_t l, char... c>
struct subSeq<i, l, tstring<c...>> {
  static_assert(i <= l);
  static_assert(l <= sizeof...(c));
  using type =
      typename subSeqImpl<0, i, l, std::integer_sequence<char, c...>>::type;
};

template <size_t, char, typename>
struct findCharIndexImpl;

template <size_t index, char target, char cur, char... c>
struct findCharIndexImpl<index, target,
                         std::integer_sequence<char, cur, c...>> {
  static constexpr size_t value =
      (cur == target
           ? index
           : findCharIndexImpl<index + 1, target,
                               std::integer_sequence<char, c...>>::value);
};

template <size_t index, char target, char c>
struct findCharIndexImpl<index, target, std::integer_sequence<char, c>> {
  static constexpr size_t value = (target == c ? index : -1);
};

template <size_t, size_t, typename>
struct findClosingBracketImpl;

template <size_t index, size_t openBracketCount, char cur, char... c>
struct findClosingBracketImpl<index, openBracketCount,
                              std::integer_sequence<char, cur, c...>> {
  static constexpr size_t value =
      (cur != '(' && cur != ')'
           ? findClosingBracketImpl<index + 1, openBracketCount,
                                    std::integer_sequence<char, c...>>::value
           : (cur == '('
                  ? findClosingBracketImpl<
                        index + 1, openBracketCount + 1,
                        std::integer_sequence<char, c...>>::value
                  : (openBracketCount == 1
                         ? index
                         : findClosingBracketImpl<
                               index + 1, openBracketCount - 1,
                               std::integer_sequence<char, c...>>::value)));
};

template <size_t index, size_t openBracketCount, char cur>
struct findClosingBracketImpl<index, openBracketCount,
                              std::integer_sequence<char, cur>> {
  static constexpr size_t value =
      (cur == ')' && openBracketCount == 1 ? index : -1);
};

template <typename>
struct stripParenthesisImpl;

template <char... c>
struct stripParenthesisImpl<tstring<c...>> {
  using type = tstring<c...>;
};

template <char... c>
struct stripParenthesisImpl<tstring<'(', c...>> {
  static constexpr size_t index =
      findClosingBracketImpl<1, 1, std::integer_sequence<char, c...>>::value;
  using type = typename std::conditional<
      index == sizeof...(c),
      typename subSeq<0, sizeof...(c) - 2, tstring<c...>>::type,
      std::integer_sequence<char, '(', c...>>::type;
};

template <typename>
struct compString;

template<typename,typename>
struct appendStringImpl;

template<char... a,char... b>
struct appendStringImpl<tstring<a...>,tstring<b...>>{
    using type = compString<tstring<a...,b...>>;
};

template<char... a,char... b>
struct appendStringImpl<tstring<a...>,compString<tstring<b...>>>{
    using type = compString<tstring<a...,b...>>;
};

template<typename,typename>
struct prependStringImpl;

template<char... a,char... b>
struct prependStringImpl<tstring<a...>,tstring<b...>>{
    using type = compString<tstring<b...,a...>>;
};

template<char... a,char... b>
struct prependStringImpl<tstring<a...>,compString<tstring<b...>>>{
    using type = compString<tstring<b...,a...>>;
};


template <char... c>
struct compString<tstring<c...>> {
  static constexpr char str[sizeof...(c) + 1] = {c..., '\0'};
  static constexpr size_t size = sizeof...(c);
  template <size_t r, size_t l>
  using substr = compString<
      typename subSeq<r, l, std::integer_sequence<char, c...>>::type>;

  using stripParenthesis =
      compString<typename stripParenthesisImpl<tstring<c...>>::type>;
  using asIntSeq = std::integer_sequence<char, c...>;

  template<typename str>
    using append = typename appendStringImpl<tstring<c...>,str>::type;
  template<typename str>
    using prepend = typename prependStringImpl<tstring<c...>,str>::type;

  template <char target, size_t start = 0>
  static constexpr size_t findCharIndex = findCharIndexImpl<
      start, target,
      typename subSeq<start, size - 1,
                      std::integer_sequence<char, c...>>::type>::value;

  template <size_t i>
  static constexpr size_t findClosingBracket = findClosingBracketImpl<
      i, 0,
      typename subSeq<i, size - 1, std::integer_sequence<char, c...>>::
          type>::value;

  template <size_t i>
  static constexpr char at = substr<i, i>::str[0];

  template <typename T>
  static constexpr std::pair<T, bool> convertTo() {
    T val;
    if (std::from_chars(&str[0], &str[size], val).ec == std::errc{})
      return {val, 1};
    else
      return {0, 0};
  }
};

template <size_t, typename, char...>
struct searchForOp;

template <size_t index, char cur, char... c, char... targets>
struct searchForOp<index, std::integer_sequence<char, cur, c...>, targets...> {
  static constexpr size_t value =
      ((cur == targets) || ...)
          ? index
          : searchForOp<index + 1, std::integer_sequence<char, c...>,
                        targets...>::value;
  static_assert(cur != '(');
};

template <size_t index, char... c, char... targets>
struct searchForOp<index, std::integer_sequence<char, '(', c...>, targets...> {
  using str = compString<tstring<'(', c...>>;
  static constexpr size_t newIndex = str::template findClosingBracket<0>;
  static_assert(newIndex <= str::size - 1);
  using newStr = str::template substr<newIndex, str::size - 1>::asIntSeq;
  static constexpr size_t value =
      searchForOp<newIndex + index, newStr, targets...>::value;
};

template <size_t index, char c, char... targets>
struct searchForOp<index, std::integer_sequence<char, c>, targets...> {
  static constexpr size_t value = -1;
};

template <typename>
struct getSplitIndex;

template <char... c>
struct getSplitIndex<compString<tstring<c...>>> {
  static constexpr size_t index() {
    constexpr size_t AddSubIndex =
        searchForOp<0, std::integer_sequence<char, c...>, '+', '-'>::value;
    if constexpr (AddSubIndex != -1) {
      return AddSubIndex;
    }
    constexpr size_t MultDivIndex =
        searchForOp<0, std::integer_sequence<char, c...>, '*', '/'>::value;
    if constexpr (MultDivIndex != -1) {
      return MultDivIndex;
    }

    constexpr size_t PowIndex =
        searchForOp<0, std::integer_sequence<char, c...>, '^'>::value;
    if constexpr (PowIndex != -1) {
      return PowIndex;
    }
    return -1;
  }
};
template <size_t Index, typename str>
struct splitByOperator {
  static constexpr size_t index = Index;
  static constexpr char op = str::template at<index>;
  using leftOperand =
      typename str::template substr<0, index - 1>::stripParenthesis;
  using rightOperand =
      typename str::template substr<index + 1, str::size - 1>::stripParenthesis;
};

template <typename str>
struct splitByOperator<0ull, str> {
  static constexpr size_t index = 0;
  static constexpr char op = str::template at<index>;
  using leftOperand = compString<tstring<>>;
  using rightOperand =
      typename str::template substr<index + 1, str::size - 1>::stripParenthesis;
};

template <typename T>
struct splitByOperator<-1ull, T> {
  static constexpr size_t index = -1;
};

template <typename str>
struct individOp {
  using res = splitByOperator<getSplitIndex<str>::index(), str>;
};

template <typename T>
constexpr T compPow(T base, T pow) {
  return pow == 0 ? 1 : base * compPow(base, pow - 1);
}

template <typename... T>
struct typeList {};

template <typename, typename>
struct prependTypeList;

template <typename T, typename... types>
struct prependTypeList<T, typeList<types...>> {
  using type = typeList<T, types...>;
};

template <typename>
struct splitVariableList;

template <size_t commaIndex, typename str>
struct splitVariableListHelper {
  using type =
      typename prependTypeList<typename str::substr<0, commaIndex - 1>,
                               typename splitVariableList<typename str::substr<
                                   commaIndex + 1, str::size - 1>>::type>::type;
};

template <typename str>
struct splitVariableListHelper<-1ull, str> {
  using type = typeList<str>;
};

template <char... c>
struct splitVariableList<compString<tstring<c...>>> {
  using str = compString<tstring<c...>>;
  static constexpr size_t commaIndex =
      searchForOp<0, std::integer_sequence<char, c...>, ','>::value;
  using type = typename splitVariableListHelper<commaIndex, str>::type;
};
template <typename T>
using extractFirstType = typename T::first_type;

template <typename... elems>
struct funcDecl {
  using map = type_map<elems...>;

  template <typename T>
  static constexpr bool hasType{
      (std::is_same_v<T, extractFirstType<elems>> || ...)};

  template <typename T>
  using findFunc = typename map::find<T>;

  template <typename pair>
  using addPair = funcDecl<pair, elems...>;
};

template <typename str>
struct isFuncCall {
  static constexpr bool value = str::template findCharIndex<'('> != -1 ? 1 : 0;
};

template <typename str>
struct splitFuncCall {
  static constexpr size_t beginIndex = str::template findCharIndex<'('>;
  static constexpr size_t endIndex =
      str::template findClosingBracket<beginIndex>;
  static_assert(endIndex != -1, "did not find ending ) in func Call");
  using name = typename str::substr<0, beginIndex - 1>;
  using variableListStr = typename str::substr<beginIndex + 1, endIndex - 1>;
  using valuelist = typename splitVariableList<variableListStr>::type;
};

template <size_t, typename, typename>
struct varIndexImpl {
  static constexpr size_t value = -1;
};

template <size_t Index, typename Target, typename T, typename... Ts>
struct varIndexImpl<Index, Target, typeList<T, Ts...>> {
  static constexpr size_t value =
      varIndexImpl<Index + 1, Target, typeList<Ts...>>::value;
};

template <size_t Index, typename Target, typename... Ts>
struct varIndexImpl<Index, Target, typeList<Target, Ts...>> {
  static constexpr size_t value = Index;
};

template <size_t Index, typename Target, typename T>
struct varIndexImpl<Index, Target, typeList<T>> {
  static constexpr size_t value = -1;
};

template <size_t Index, typename Target>
struct varIndexImpl<Index, Target, typeList<Target>> {
  static constexpr size_t value = Index;
};

template <size_t, size_t, typename>
struct typeListAtImpl;

template <size_t i, size_t target, typename T, typename... Ts>
struct typeListAtImpl<i, target, typeList<T, Ts...>> {
  using type = typeListAtImpl<i + 1, target, typeList<Ts...>>::type;
};

template <size_t i, size_t target, typename T>
struct typeListAtImpl<i, target, typeList<T>> {
  using type = T;
};

template <size_t Target, typename T, typename... Ts>
struct typeListAtImpl<Target, Target, typeList<T, Ts...>> {
  using type = T;
};

template <size_t Target, typename T>
struct typeListAtImpl<Target, Target, typeList<T>> {
  using type = T;
};

template <typename expr, typename VL>
struct mathFuncInfo {
  using ExprType = expr;
  using VariableList = VL;
};

template <typename VL, typename TL, typename FD>
struct execContext {
  using valueList = VL;
  using varList = TL;
  using funcDecl = FD;

  template <typename T>
  static constexpr size_t varIndex = varIndexImpl<0, T, TL>::value;

  template <size_t index>
  using getValueType = typename typeListAtImpl<0, index, VL>::type;

  template <typename T>
  static constexpr bool isInContext = FD::template hasType<T>;

  template <typename T>
  using pairOfType = typename FD::template findFunc<T>;
  template <typename pair>
  using addDecl = execContext<VL, TL, typename FD::addPair<pair>>;
};

using emptyExecContext =
    execContext<typeList<void>, typeList<void>, funcDecl<typePair<void, void>>>;

template <typename, typename T = int, typename = emptyExecContext>
struct mathEngineThing;

template <typename T, typename context>
struct mathEngineThing<compString<tstring<>>, T, context> {
  static constexpr T calc() { return 0; }
};

template <typename, typename, typename>
struct evaluateValueList;

template <typename value, typename... other, typename T, typename context>
struct evaluateValueList<typeList<value, other...>, T, context> {
  static constexpr T val = mathEngineThing<value, T, context>::calc();
  using type = typename prependTypeList<
      std::integral_constant<T, val>,
      typename evaluateValueList<typeList<other...>, T, context>::type>::type;
};

template <typename value, typename T, typename context>
struct evaluateValueList<typeList<value>, T, context> {
  static constexpr T val = mathEngineThing<value, T, context>::calc();
  using type = typeList<std::integral_constant<T, val>>;
};

template <typename T, char... c, typename context>
struct mathEngineThing<compString<tstring<c...>>, T, context> {
  using str = compString<tstring<c...>>::stripParenthesis;
  using res = individOp<str>::res;
  static constexpr T calc() {
    // check it it is an expression or not
    if constexpr (res::index == -1) {
      // it is not an expression
      // 1) check if it is a number
      // 2) check if it is a variable
      // 3) check if it is a funcCall
      // 4) check if it is a decl constant
      constexpr auto num = str::template convertTo<T>();
      if constexpr (num.second) {
        return num.first;
      } else
        // not a number, check if it is a variable
        if constexpr (context::template varIndex<str> != -1) {
          constexpr size_t i = context::template varIndex<str>;
          using varType = typename context::template getValueType<
              context::template varIndex<str>>;
          return varType::value;
        } else
          // not a variable, check if it is a funcCall
          if constexpr (isFuncCall<str>::value) {
            using split = splitFuncCall<str>;
            if constexpr (context::template isInContext<typename split::name>) {
              using funcInfo =
                  typename context::template pairOfType<typename split::name>;
              using evaluatedValueList =
                  typename evaluateValueList<typename split::valuelist, T,
                                             context>::type;
              using newContext = execContext<evaluatedValueList,
                                             typename funcInfo::VariableList,
                                             typename context::funcDecl>;
              return mathEngineThing<typename funcInfo::ExprType, T, newContext>::calc();
            } else {
              throw "func call not found in context";
              return 0;
            }
          }
          // not a funcCall, must be a constant variable if anything
          else {
            if constexpr (context::template isInContext<str>) {
              using expr = typename context::template pairOfType<str>;
              using newContext =
                  execContext<void, void, typename context::funcDecl>;
              return mathEngineThing<typename expr::ExprType, T, newContext>::calc();
            } else {
              throw "constant var not found in context";
              return 0;
            }
          }
    } else {
      // it is an expression
      T left = mathEngineThing<typename res::leftOperand, T, context>::calc();
      T right = mathEngineThing<typename res::rightOperand, T, context>::calc();
      switch (res::op) {
        case '+':
          return left + right;
        case '-':
          return left - right;
        case '*':
          return left * right;
        case '/':
          return left / right;
        case '^':
          return compPow(left, right);
      };
    }
  }
};

template <size_t openningBracket, typename str>
struct splitMathFuncImpl {
  static constexpr size_t closingBracket =
      str::template findClosingBracket<openningBracket>;
  static constexpr size_t equalPos = str::template findCharIndex<'='>;

  using name = typename str::substr<0, openningBracket - 1>;
  using variableListStr =
      typename str::substr<openningBracket + 1, closingBracket - 1>;
  using variableList = typename splitVariableList<variableListStr>::type;

  using expr = typename str::substr<equalPos + 1, str::size - 1>;
};

template <typename str>
struct splitMathFuncImpl<-1ull, str> {
  static constexpr size_t equalPos = str::template findCharIndex<'='>;
  using variableList = typeList<>;
  using name = typename str::substr<0, equalPos - 1>;
  using expr = typename str::substr<equalPos + 1, str::size - 1>;
};

template <typename str>
struct splitMathFunc {
  static constexpr size_t openningBracket = str::template findCharIndex<'('>;
  static constexpr size_t equalPos = str::template findCharIndex<'='>;
  using inter = std::conditional<(openningBracket < equalPos),
                                 splitMathFuncImpl<openningBracket, str>,
                                 splitMathFuncImpl<-1ull, str>>::type;

  using name = typename inter::name;
  using variableList = typename inter::variableList;
  using expr = typename inter::expr;
};

template <typename Tcontext = emptyExecContext>
struct userContext {
  using context = Tcontext;

  template <typename str>
  using add = userContext<typename context::addDecl<typePair<
      typename splitMathFunc<compString<str>>::name,
      mathFuncInfo<typename splitMathFunc<compString<str>>::expr,
                   typename splitMathFunc<compString<str>>::variableList>>>>;
};



template<typename>
struct preprocessString;

template<size_t index,typename str>
struct preprocessStringInter{

};
template<typename str>
struct preprocessStringInter<-1ull,str>{
    using type = str;
};

template<char... c>
struct preprocessString<tstring<c...>>{
    using str = compString<tstring<c...>>;
    static constexpr size_t index = searchForOp<0,tstring<c...>,'-'>::value;
};
}
using detail::userContext;
template <typename, typename = detail::emptyExecContext>
constexpr int calcString = 0;

template <char... c>
constexpr int calcString<tstring<c...>> =
    detail::mathEngineThing<detail::compString<tstring<c...>>, int>::calc();

template <char... c, typename context>
constexpr int calcString<tstring<c...>, detail::userContext<context>> =
    detail::mathEngineThing<detail::compString<tstring<c...>>, int, context>::calc();
}

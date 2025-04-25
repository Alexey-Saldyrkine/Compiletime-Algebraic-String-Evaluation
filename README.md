# Compiletime Algebraic String Evaluation
This is a single header that provides the capability to evaluate algeraic expressions at compiletime.

This is done using main three types, located in namespace calcStringNS.
- tstring, is a type that represents an expression.
- userContext, is a type that contains delerations of algebraic expressions, that can be used in other expressions.
- calcString, is the type that evaluates an expression. 

## tstring and how expressions are decleard

tstring is a wrapper around std::integer_subsequence<char,chrs...>.<br>
A string of characters can be converted to a tstring value through the user-defined literal _tstr. Than covnerted to a type using decltype.<br>

example: using expr = decltype("2+5"_tstr);<br>

expr is an tstring type that represents the expression 2+5, which appon evaluation will result in 7.

## userContext

userContext is a type that hold information about other expression definitions that could be used in an evalution of an expression.<br>

### What is a definition
A definition is a sub set of expressions that define either a function or a constant.<br>
A definition must contain one '=' character.<br>
__Note:__ for complete syntex rules see !!!!INSERTLINKHERE!!!!

### Value type
When declearing a userContext, an optinal value type T can be provided.<br>
Value type T will be the type all numbers will be held in during evaluation, and the result of evaluation will be returned as type T.<br>
If no type T is provided, then type T defualts to int.<br>
examples:<br>

using contextA = userContext<>; //value type T = int

using contextB = userContext\<long int>; // value type T = long int

__NOTE:__ currently only integer types can be used as value types;

### Adding definitions to a context

userContext by defualt holds no expression definitions.<br>
New defintions can be added through the member type "add" of userConext.<br>
This will return a new UserContext that contains all old and the new definitions<br>
__Note:__ only definitions can be added to a context.<br>
example:<br>

using contextA = userContext<>::add\<def1>; // contains def1

using contextB = contextA::add\<def2>; // contains def1 and def2

such "add"s can be chained in a single using decleration:

using context = userConext<>::add\<def1>::add\<def2>::add\<def3>; // contains def1, def2 and def3;




## requirments and limitations
For the conversion of a number represented by a string of chars, constexpr std::from_chars is used.
defiened in header <[charconv](https://en.cppreference.com/w/cpp/utility/from_chars)>.
- As such C++23 is required for constexpr std::from_chars.
- As such only integer types can be used as the value type during evaluation.

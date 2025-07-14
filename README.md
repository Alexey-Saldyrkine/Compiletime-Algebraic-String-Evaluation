# Compiletime Algebraic String Evaluation
This is a single header that provides the capability to evaluate algeraic expressions at compiletime.

## Example
```cpp
using namespace calcStringNS;
using context = userContext<>
::add<decltype("a=2"_tstr)>
::add<decltype("f(x)=2*x+x"_tstr)>
::add<decltype("g(x)=f(x)+f(1)"_tstr)>;
// declare the context the evaluation takes place in.

using expr = decltype("f(a)+g(a)"_tstr); // the expression that will be evaluated.

constexpr int exprValue = calcString<expr,context>;

static_assert(exprValue == 15); // exprValue is evaluated at compile time


```

[godBolt link](https://godbolt.org/z/1qPhsbfKz)

This is done using main three types, located in namespace calcStringNS.
- tstring, is a type that represents an expression.
- userContext, is a type that contains definitions of algebraic expressions, that can be used in other expressions.
- calcString, is the type that evaluates an expression. 

## tstring and how expressions are decleard

tstring is a wrapper around std::integer_subsequence<char,chrs...>.<br>
A string of characters can be converted to a tstring value through the user-defined literal _tstr. Then covnerted to a type using decltype.<br>

example: using expr = decltype("2+5"_tstr);<br>

expr is an tstring type that represents the expression 2+5, which appon evaluation will result in 7.

__Note:__ for syntex rules see !!!!INSERTLINKHERE!!!

## userContext

userContext is a type that hold information about other expression definitions that could be used in an evalution of an expression.<br>

### What is a definition
A definition is an expressions that define either a function or a constant.<br>
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

## calcString and how to evaluate expressions

Expressions can be evaluated to thier result value uaing the calcString template value.<br>
calcString will return a value of type 'value type';

### Value type 
If a userContext is given as the second paramater, then the value type is got from the userContext.<br>
If a value type is given as the second paramater, then it will be used.<br>
If no second paramater is given, then the value type defualts to int.<br>
examples:

constexpr int res = calcString\<expr>; // the expreassion expr will be evaluated with the default value type int.

constexpr long int res = calcString\<expr,long int>; // the expreassion expr will be evaluated with the value type being long int.

using context = userContext\<long long int>;<br>
constexpr long long int res = calcString\<expr,context>; // the expreassion expr will be evaluated with the value type of context (i.e. long long int).

### Detail

## Order of evaluation

Evaluation of an expression is done by the mathEngine.<br>
When the mathEngine recives an expression the following steps will happen:
- The expression will be searched for the leftmost operator in order of operators precedence.<br>
  - If an operator is found, than the expression will split by the oprator into 2 operand expressions.<br>
    Each operand expression is evaluated by the math engine into operand values, with the current context.<br>
    The operator is applied to the operand value and the result it returned.<br>
- If no operator is found, then the expression is a vlaue expression.<br>
1. The value expresstion is attempted to be converted int a value of type 'value type;.<br>
    - If the converstion is successful, then the value is returned
2. Else, if this expression originates from a function call, the expression is tested to see if its a variable of the function.
    - If it is, than the corresponding value of the function calls value list is returned.<br>
3. The expression is tested for the presence of the '(' character, if it is present, then the expression is considered a function call.<br>
    - The expression is split into the function name and a list of paramaters. If the function is present in the context, then all the paramaters are evaluated in order into the value list.<br>
      The corresponding expression of the function is got from the context, it is evaluated with the created value list. The value is returned.<br>
    - If the function is not found in the context, then an error occurs.<br>
4. Else, the expression must be a constant.
    - If the expression is present in the context, then the corresponding expression is evaluated and returned.
    - If it is not present, then an errot occurs.

__Note:__ an empty expression (i.e. "") evaluates to 0.<br>
This is used primeraly to define negative numbers in an expression. i.e. the expression "-1" is treated as "0-1" which evaluates to -1.

## Operator and operator precedence
operator precedence:
1. '+' '-'
2. '*' '/'
3. '^'
4. '(' ')'
Operators of the same level are evaluated left to right as they apear in an expression.

# syntex rules

## expression

### numbers
An expression is considered a number if it can be converted to a value of type 'value type'.<br>
__Note:__ At present std::from_chars is used to determine whether an expression is a number.<br>

### function calls
The pattern of a function call is "\[function name](\[expr1],\[expr2],...,\[exprN])".<br>
Where \[function name] is is the name of a function definition in the context, that is will be evaluated in.<br>
Where \[exprN] is the expression that will be evaluated to the Nth value in the function call value list.<br>

## definetions

### constants

The pattern of a constant is as such: "\[name of constant]=\[expression]";<br>
Where \[name of constant] is a string of characters that will identifie the constant.<br>
- The name can contain any characters other then operators and '='.<br>
- The name must be unique in the context it is added too.<br>

And \[expression] is the expression the constant will be evaluated to.<br>
\[expression] must be a vaild string of characters that can be evaluated by calcString.<br>
The \[expression] can refer to other constants and functions, so long as they are defiened in the same context, by the time that the context is used by calcString.

### functions

The pattern of a function is as such: "\[name of function](\[var1],\[var2],...,\[varN])=\[expression]".<br>
Where \[name of function] is a string of characters that will identify the function.<br>
- The name can contain any characters other then operators and '='.<br>
- The name must be unique in the context it is added too.<br>

Where /[varN] is a string of characters that will identify a variable of the function.<br>
- The variable name can contain any characters other then operators and '='.<br>
During evaluation /[varN] will evaluate to Nth variable value of a function call.<br>
The amount of varable must be more than zero.<br>

And \[expression] is the expression the function will be evaluated to.<br>
If the expression contains Nth variable name of its function, then the variable will we evaluated to the Nth value of the function call.




## requirments and limitations
For the conversion of a number represented by a string of chars, constexpr std::from_chars is used.
defiened in header <[charconv](https://en.cppreference.com/w/cpp/utility/from_chars)>.
- As such C++23 is required for constexpr std::from_chars.
- As such only integer types can be used as the value type during evaluation.

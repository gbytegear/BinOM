## How contribute to this project?
### Reporting Bugs
If you found some problem with working of this project you can send bug report to [issues of this repository](https://github.com/gbytegear/BinOM/issues).
First, make sure that the error was not found and published earlier.
Please try to clearly describe the error in the work of the project and, if possible, describe the complete algorithm for receiving the error that you saw. 

### Pull Requests
If you want to help fix bugs in the project or add some functionality,
first write a new message to [issues of this repository](https://github.com/gbytegear/BinOM/issues) with a description of what you are going to do.

When writing or correcting project code, follow the style defined [below](#code-styleguide).

## Code styleguide
### Compound words style
In C-like programming languages such as JavaScript, C ++ and others, try to style the code in a uniform way:
* **PascalCase** - classes, structs, unions, enums - anything that is a data type.

Example:
```cpp
class ClassName {/*...*/};
struct StructName {/*...*/};
union UnionName {/*...*/};
enum EnumName {/*...*/};
```
* **snake_case** - variables, class fields, structs fields, unions fields, functions argumments, methods argumments - everything that can be stored and changed in memory.

Example:
```cpp
int some_variable;
class ClassName {
  int some_field;
};
void doSomething(int some_argumment) {/*...*/}
```
* **camelCase** - functions, methods, lamdas, functor instaces - everything that can be executed. Try to put the verb first word.

Example:
```cpp
void doSomething() {/*...*/}
class ClassName {
  int getSomething();
  int setSomething(int something);
};
struct SomeFunctor {
  void operator() {/*...*/}
} doSome;
```
* **UPPER_CASE_SNAKE_CASE** - constants and macroses - everything that cannot be changed while the program is running.

Example:
```cpp
const int SOME_NUMBER = 42;
constexpr int SOME_OTHER_NUMBER = 24;
#define SOME_MACRO
```

Try to write names that reflect the essence of the object. Try to style the compound words to describe the type of statement.

### Parentheses
Do not put statement parentheses immediately after statements or object definitions. Separate them with a space.
Place an open statement parenthesis on the same line as the statement or object definition.
```cpp
if(/*condition*/) {
  /*expressions*/
}

while(/*condition*/) {
  /*expressions*/
}

for(/*definitions*/;/*condition*/;/*counter change*/) {
  /*expressions*/
}

void doSomething() {
  /*expressions*/
}
```
In cases where it is possible not to write operator brackets, place the expression after the operator.
```cpp
if(/*condition*/) /*expression*/

while(/*condition*/) /*expression*/

for(/*definitions*/;/*condition*/;/*counter change*/) /*expression*/
```
If the expression is too long to fit on the same line with the statement, it is allowed to fit the expression on a new line.
```cpp
if(/*condition*/)
  /*is too long expression to fit on the same line with the statement*/

while(/*condition*/)
  /*is too long expression to fit on the same line with the statement*/

for(/*definitions*/;/*condition*/;/*counter change*/)
  /*is too long expression to fit on the same line with the statement*/
```
In functions and methods with short expressions in statement brackets, it is allowed to place expressions and a closing parenthesis on the same line on which the function or method was declared.
```cpp
void doSomething() { /*expressions*/ }
```

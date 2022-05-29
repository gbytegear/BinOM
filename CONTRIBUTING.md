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
In C-like programming languages such as JavaScript, C++ and others, try to style the code in a uniform way:
* **PascalCase** - classes, structs, unions, enums - anything that is a data type.
Example: 
```cpp
class ClassName {/*...*/};
struct StructName {/*...*/};
union UnionName {/*...*/};
enum EnumName {/*...*/};
```
Exception: type traits must be written in **snake_case**(as in STL). Example:
```cpp
template <template<typename T> class CRTP_Base, class CRTP_Driven>
struct is_crtp_base_of : std::is_base_of<CRTP_Base<remove_cvref_t<CRTP_Driven>>, remove_cvref_t<CRTP_Driven>> {};

template <template<typename T> class CRTP_Base, class CRTP_Driven>
inline constexpr bool is_crtp_base_of_v = is_crtp_base_of<CRTP_Base, CRTP_Driven>::value;
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
Exception: extending C++ syntax
```cpp
#define self (*this)        // T* this -> T& this
#define elif else if        // if(condition) {...} elif(condition) {...} else {...} -> if(condition) {...} else if(condition) {...} else {...}
#define forever while(1)    // forever {... if(condition) break; ...} -> while(1) {... if(condition) break; ...}
#define ever (;;)           // for ever {... if(condition) break; ...} -> for (;;) {... if(condition) break; ...}
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

for(/*counter definition*/;/*condition*/;/*counter change*/) {
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

for(/*counter definition*/;/*condition*/;/*counter change*/) /*expression*/
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
void doSomething() { /*expression*/ }
```

### Namespaces (Specific for libraries/frameworks development)
All classes, functions, methods, and other elements must be in the namespace named by the name of the project. Example:
```cpp
namespace project_name {
// All class and functions
}
```
**TO AVOID NAME CONFLICTS WITH SIDE PROJECTS ELEMENTS, IT IS STRICTLY FORBIDDEN TO PUT ELEMENTS IN THE GLOBAL NAMESPACE**

Recomendation: If the element is not intended to be used in user code, it must be placed in a subnamespace called "priv"
```cpp
namespace project_name {
namespace priv {
// Some private elements
}
// Some public elements
}
// Or
namespace project_name::priv {
// Some private elements
}
```

Exception: If some elements involved in the project can be used separately from the project, it is recommended to place them in a separate non-nested namespace. Example:
```cpp
namespace extended_type_traits {
// ...
template <class T, class U>
inline constexpr bool is_base_of_v = is_base_of<T, U>::value;

template <template<typename T> class CRTP_Base, class CRTP_Driven>
struct is_crtp_base_of : std::is_base_of<CRTP_Base<remove_cvref_t<CRTP_Driven>>, remove_cvref_t<CRTP_Driven>> {};
// ...
}
```

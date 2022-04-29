#include "function_type_traits.h"

[[maybe_unused]] static void FreeFunction() {}
[[maybe_unused]] static void FreeFunction2() {}

using FreeFunctionPointerType = void(*)();
using FreeFunctionType = void();
static_assert(std::is_same_v<function_type_traits<FreeFunctionPointerType>::identity, function_type_traits<FreeFunctionType>::identity>);
static_assert(std::is_same_v<decltype(FreeFunction), FreeFunctionType>);
static_assert(std::is_same_v<function_traits<FreeFunction>::identity, function_type_traits<FreeFunctionType>::identity>);
static_assert(std::is_same_v<function_traits<&FreeFunction>::identity, function_type_traits<FreeFunctionType>::identity>);

[[maybe_unused]] static int FreeFunctionRet() {return 0;}
[[maybe_unused]] static void FreeFunctionArgs(char, int, double*) {}
[[maybe_unused]] static void FreeFunctionVariadic(...) {}
[[maybe_unused]] static void FreeFunctionVariadicArgs(char, int, double*, ...) {}

struct MyClass
{
	void MemberFunction() {}
	int MemberFunctionRet() { return 0; }
	void MemberFunctionArgs(char, int, double*) {}
	void MemberFunctionVariadic(...) {}
	void MemberFunctionVariadicArgs(char, int, double*, ...) {}

	void MemberFunctionNoexcept() noexcept;

	void MemberFunctionConst() const;
	void MemberFunctionVolatile() volatile;

	void MemberFunctionLRef() &;
	void MemberFunctionRRef() &&;

	[[maybe_unused]] static void MemberFunctionStatic() {}
};

// Validate two functions with the same signature have the same identity
static_assert(std::is_same_v<function_traits<FreeFunction>::identity, function_traits<FreeFunction2>::identity>);

// Validate non-static member functions have class_type set
static_assert(std::is_same_v<function_traits<&FreeFunction>::class_type, void>);
static_assert(std::is_same_v<function_traits<&MyClass::MemberFunction>::class_type, MyClass>);
static_assert(std::is_same_v<function_traits<&MyClass::MemberFunctionStatic>::class_type, void>);

// Validate cv, ref and noexcept qualifiers
static_assert(function_traits<&MyClass::MemberFunction>::noexcept_v == false);
static_assert(function_traits<&MyClass::MemberFunctionNoexcept>::noexcept_v == true);
static_assert(function_traits<&MyClass::MemberFunction>::const_qualified == false);
static_assert(function_traits<&MyClass::MemberFunctionConst>::const_qualified == true);
static_assert(function_traits<&MyClass::MemberFunction>::const_qualified == false);
static_assert(function_traits<&MyClass::MemberFunctionVolatile>::volatile_qualified == true);
static_assert(function_traits<&MyClass::MemberFunction>::lref_qualified == false);
static_assert(function_traits<&MyClass::MemberFunctionLRef>::lref_qualified == true);
static_assert(function_traits<&MyClass::MemberFunction>::rref_qualified == false);
static_assert(function_traits<&MyClass::MemberFunctionRRef>::rref_qualified == true);


// Validate variadic flag
static_assert(function_traits<&FreeFunction>::variadic == false);
static_assert(function_traits<&FreeFunctionVariadic>::variadic == true);
static_assert(function_traits<&FreeFunctionVariadicArgs>::variadic == true);
static_assert(function_traits<&MyClass::MemberFunction>::variadic == false);
static_assert(function_traits<&MyClass::MemberFunctionVariadic>::variadic == true);
static_assert(function_traits<&MyClass::MemberFunctionVariadicArgs>::variadic == true);

// Validate function arity
static_assert(function_traits<&FreeFunction>::arity == 0);
static_assert(function_traits<&FreeFunctionVariadic>::arity== 0);
static_assert(function_traits<&FreeFunctionArgs>::arity == 3);
static_assert(function_traits<&FreeFunctionVariadicArgs>::arity == 3);
static_assert(function_traits<&MyClass::MemberFunction>::arity == 0);
static_assert(function_traits<&MyClass::MemberFunctionVariadic>::arity== 0);
static_assert(function_traits<&MyClass::MemberFunctionArgs>::arity == 3);
static_assert(function_traits<&MyClass::MemberFunctionVariadicArgs>::arity == 3);

// Validate function argument types
static_assert(std::is_same_v<function_traits<&FreeFunctionArgs>::arg_t<0>, char>);
static_assert(std::is_same_v<function_traits<&FreeFunctionArgs>::arg_t<1>, int>);
static_assert(std::is_same_v<function_traits<&FreeFunctionArgs>::arg_t<2>, double*>);
static_assert(std::is_same_v<function_traits<&FreeFunctionVariadicArgs>::arg_t<0>, char>);
static_assert(std::is_same_v<function_traits<&FreeFunctionVariadicArgs>::arg_t<1>, int>);
static_assert(std::is_same_v<function_traits<&FreeFunctionVariadicArgs>::arg_t<2>, double*>);
// These fail to compile with out-of-bounds indices
//static_assert(std::is_same_v<function_traits<&FreeFunctionArgs>::arg_t<-1>, void>);
//static_assert(std::is_same_v<function_traits<&FreeFunctionArgs>::arg_t<3>, void>);
//static_assert(std::is_same_v<function_traits<&FreeFunctionVariadicArgs>::arg_t<-1>, void>);
//static_assert(std::is_same_v<function_traits<&FreeFunctionVariadicArgs>::arg_t<3>, void>);


// Validate function return types
static_assert(std::is_same_v<function_traits<&FreeFunction>::return_type, void>);
static_assert(std::is_same_v<function_traits<&MyClass::MemberFunction>::return_type, void>);
static_assert(std::is_same_v<function_traits<&FreeFunctionRet>::return_type, int>);
static_assert(std::is_same_v<function_traits<&MyClass::MemberFunctionRet>::return_type, int>);


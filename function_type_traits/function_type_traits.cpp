#include "function_type_traits.h"
// import function_type_traits;
#include <type_traits>
#include <cstdio>
#include <cmath>
#include <exception>

using namespace function_type_traits;

// Validate examples from README.md
static_assert(std::is_same_v<fn_traits<&fminf>::identity, fn_type_traits<decltype(&fminf)>::identity>);
static_assert(std::is_same_v<fn_traits<&fminf>::identity, fn_traits<&fmaxf>::identity>);
static_assert(std::is_same_v<fn_traits<&printf>::arg_t<0>, const char*>);

using fminf_traits = fn_traits<&fminf>;
static_assert(std::is_same_v<fminf_traits::return_type, float>);
static_assert(std::is_same_v<fminf_traits::arg_types, std::tuple<float, float>>);
static_assert(fminf_traits::arity == 2);
static_assert(fminf_traits::variadic == false);

using printf_traits = fn_traits<&printf>;
static_assert(std::is_same_v<printf_traits::return_type, int>);
static_assert(std::is_same_v<printf_traits::arg_types, std::tuple<const char*>>);
static_assert(printf_traits::arity == 1);
static_assert(printf_traits::variadic == true);

struct cls
{
	void MemberFunctionArgs(char, int, double*) {}
	void MemberFunctionConst() const {}
	static void MemberFunctionStatic() {}
};

using arg_fn_traits = fn_traits<&cls::MemberFunctionArgs>;
using const_fn_traits = fn_traits<&cls::MemberFunctionConst>;
using static_fn_traits = fn_traits<&cls::MemberFunctionStatic>;


// Extract class type
static_assert(std::is_same_v<arg_fn_traits::class_type, cls>);
static_assert(std::is_same_v<static_fn_traits::class_type, void>);

// Extract Function Arguments
static_assert(std::is_same_v<arg_fn_traits::arg_types, std::tuple<char, int, double*>>);
static_assert(std::is_same_v<arg_fn_traits::arg_t<0>, char>);
static_assert(std::is_same_v<arg_fn_traits::arg_t<1>, int>);
static_assert(std::is_same_v<arg_fn_traits::arg_t<2>, double*>);

// Extract const-qualifier
static_assert(arg_fn_traits::const_qualified == false);
static_assert(const_fn_traits::const_qualified == true);



[[maybe_unused]] static void FreeFunction() {}
[[maybe_unused]] static void FreeFunction2() {}

using FreeFunctionPointerType = void(*)();
using FreeFunctionType = void();
static_assert(std::is_same_v<fn_type_traits<FreeFunctionPointerType>::identity, fn_type_traits<FreeFunctionType>::identity>);
static_assert(std::is_same_v<decltype(FreeFunction), FreeFunctionType>);
static_assert(std::is_same_v<fn_traits<FreeFunction>::identity, fn_type_traits<FreeFunctionType>::identity>);
static_assert(std::is_same_v<fn_traits<&FreeFunction>::identity, fn_type_traits<FreeFunctionType>::identity>);

[[maybe_unused]] static int FreeFunctionRet() { return 0; }
[[maybe_unused]] static void FreeFunctionArgs(char, int, double*) {}
[[maybe_unused]] static void FreeFunctionVariadic(...) {}
[[maybe_unused]] static void FreeFunctionVariadicArgs(char, int, double*, ...) {}

struct MyClass
{
	MyClass(){}
	[[maybe_unused]] void MemberFunction() {}
	[[maybe_unused]] int MemberFunctionRet() { return 0; }
	[[maybe_unused]] void MemberFunctionArgs(char, int, double*) {}
	[[maybe_unused]] void MemberFunctionVariadic(...) {}
	[[maybe_unused]] void MemberFunctionVariadicArgs(char, int, double*, ...) {}

	[[maybe_unused]] void MemberFunctionNoexcept() noexcept {}

	[[maybe_unused]] void MemberFunctionConst() const {}
	[[maybe_unused]] void MemberFunctionVolatile() volatile {}

	[[maybe_unused]] void MemberFunctionLRef()& {}
	[[maybe_unused]] void MemberFunctionRRef()&& {}

	[[maybe_unused]] static void MemberFunctionStatic() {}
};

// Validate two functions with the same signature have the same identity
static_assert(std::is_same_v<fn_traits<FreeFunction>::identity, fn_traits<FreeFunction2>::identity>);

// Validate non-static member functions have class_type set
static_assert(std::is_same_v<fn_traits<&FreeFunction>::class_type, void>);
static_assert(std::is_same_v<fn_traits<&MyClass::MemberFunction>::class_type, MyClass>);
static_assert(std::is_same_v<fn_traits<&MyClass::MemberFunctionStatic>::class_type, void>);

// Validate const, voliatile, ref and noexcept qualifiers
static_assert(fn_traits<&MyClass::MemberFunction>::noexcept_v == false);
static_assert(fn_traits<&MyClass::MemberFunctionNoexcept>::noexcept_v == true);
static_assert(fn_traits<&MyClass::MemberFunction>::const_qualified == false);
static_assert(fn_traits<&MyClass::MemberFunctionConst>::const_qualified == true);
static_assert(fn_traits<&MyClass::MemberFunction>::const_qualified == false);
static_assert(fn_traits<&MyClass::MemberFunctionVolatile>::volatile_qualified == true);
static_assert(fn_traits<&MyClass::MemberFunction>::lref_qualified == false);
static_assert(fn_traits<&MyClass::MemberFunctionLRef>::lref_qualified == true);
static_assert(fn_traits<&MyClass::MemberFunction>::rref_qualified == false);
static_assert(fn_traits<&MyClass::MemberFunctionRRef>::rref_qualified == true);


// Validate variadic flag
static_assert(fn_traits<&FreeFunction>::variadic == false);
static_assert(fn_traits<&FreeFunctionVariadic>::variadic == true);
static_assert(fn_traits<&FreeFunctionVariadicArgs>::variadic == true);
static_assert(fn_traits<&MyClass::MemberFunction>::variadic == false);
static_assert(fn_traits<&MyClass::MemberFunctionVariadic>::variadic == true);
static_assert(fn_traits<&MyClass::MemberFunctionVariadicArgs>::variadic == true);

// Validate function arity
static_assert(fn_traits<&FreeFunction>::arity == 0);
static_assert(fn_traits<&FreeFunctionVariadic>::arity== 0);
static_assert(fn_traits<&FreeFunctionArgs>::arity == 3);
static_assert(fn_traits<&FreeFunctionVariadicArgs>::arity == 3);
static_assert(fn_traits<&MyClass::MemberFunction>::arity == 0);
static_assert(fn_traits<&MyClass::MemberFunctionVariadic>::arity== 0);
static_assert(fn_traits<&MyClass::MemberFunctionArgs>::arity == 3);
static_assert(fn_traits<&MyClass::MemberFunctionVariadicArgs>::arity == 3);

// Validate function argument types
static_assert(std::is_same_v<fn_traits<&FreeFunctionArgs>::arg_types, std::tuple<char, int, double*>>);
static_assert(std::is_same_v<fn_traits<&FreeFunctionArgs>::arg_t<0>, char>);
static_assert(std::is_same_v<fn_traits<&FreeFunctionArgs>::arg_t<1>, int>);
static_assert(std::is_same_v<fn_traits<&FreeFunctionArgs>::arg_t<2>, double*>);
static_assert(std::is_same_v<fn_traits<&FreeFunctionVariadicArgs>::arg_types, std::tuple<char, int, double*>>);
static_assert(std::is_same_v<fn_traits<&FreeFunctionVariadicArgs>::arg_t<0>, char>);
static_assert(std::is_same_v<fn_traits<&FreeFunctionVariadicArgs>::arg_t<1>, int>);
static_assert(std::is_same_v<fn_traits<&FreeFunctionVariadicArgs>::arg_t<2>, double*>);
// These fail to compile with out-of-bounds indices
//static_assert(std::is_same_v<fn_traits<&FreeFunctionArgs>::arg_t<-1>, void>);
//static_assert(std::is_same_v<fn_traits<&FreeFunctionArgs>::arg_t<3>, void>);
//static_assert(std::is_same_v<fn_traits<&FreeFunctionVariadicArgs>::arg_t<-1>, void>);
//static_assert(std::is_same_v<fn_traits<&FreeFunctionVariadicArgs>::arg_t<3>, void>);


// Validate function return types
static_assert(std::is_same_v<fn_traits<&FreeFunction>::return_type, void>);
static_assert(std::is_same_v<fn_traits<&MyClass::MemberFunction>::return_type, void>);
static_assert(std::is_same_v<fn_traits<&FreeFunctionRet>::return_type, int>);
static_assert(std::is_same_v<fn_traits<&MyClass::MemberFunctionRet>::return_type, int>);


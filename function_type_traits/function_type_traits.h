#pragma once
#include <type_traits>
#include <tuple>

namespace function_type_traits
{
	namespace detail
	{
		template <typename T>
		struct function_type_traits_impl
		{
			static_assert(std::is_function_v<T>);
		};

		template <typename ReturnType>
		struct return_traits
		{
			using return_type = ReturnType;
		};

		template <bool NoExcept>
		struct exception_traits
		{
			constexpr static bool noexcept_v = NoExcept;
		};

		template <bool Variadic, typename... Args>
		struct arg_traits
		{
			constexpr static int arity = sizeof...(Args);
			using arg_types = std::tuple<Args...>;

			template<int ArgIndex>
			using arg_t = std::tuple_element_t<ArgIndex, arg_types>;
			constexpr static bool variadic = Variadic;
		};

		template <
			typename ClassType,
			bool Const,
			bool Volatile,
			bool LRef,
			bool RRef
		>
			struct member_fn_traits
		{
			using class_type = ClassType;
			constexpr static bool const_qualified = Const;
			constexpr static bool volatile_qualified = Volatile;
			constexpr static bool lref_qualified = LRef;
			constexpr static bool rref_qualified = RRef;
		};

		struct static_fn_traits : member_fn_traits<void, false, false, false, false> {};

		template <typename ReturnType, typename... Args>
		struct function_type_traits_impl<ReturnType(Args...)>
			: return_traits<ReturnType>
			, arg_traits<false, Args...>
			, static_fn_traits
			, exception_traits<false>
		{
		};

		template <typename ReturnType, typename... Args>
		struct function_type_traits_impl<ReturnType(Args...) noexcept>
			: return_traits<ReturnType>
			, arg_traits<false, Args...>
			, static_fn_traits
			, exception_traits<true>
		{
		};

		template <typename ReturnType, typename... Args>
		struct function_type_traits_impl<ReturnType(Args..., ...)>
			: return_traits<ReturnType>
			, static_fn_traits
			, arg_traits<true, Args...>
			, exception_traits<false>
		{
		};

		template <typename ReturnType, typename... Args>
		struct function_type_traits_impl<ReturnType(Args..., ...) noexcept>
			: return_traits<ReturnType>
			, static_fn_traits
			, arg_traits<true, Args...>
			, exception_traits<true>
		{
		};


		#define MemberFunction(_const, _volitile, _lref, _rref, ...) \
			template <typename ReturnType, typename ClassType, typename... Args> \
			struct function_type_traits_impl<ReturnType(ClassType::*)(Args...) __VA_ARGS__> \
				: return_traits<ReturnType> \
				, member_fn_traits<ClassType, _const, _volitile, _lref, _rref> \
				, arg_traits<false, Args...> \
				, exception_traits<false> \
			{ \
			};\
			template <typename ReturnType, typename ClassType, typename... Args> \
			struct function_type_traits_impl<ReturnType(ClassType::*)(Args...) __VA_ARGS__ noexcept> \
				: return_traits<ReturnType> \
				, member_fn_traits<ClassType, _const, _volitile, _lref, _rref> \
				, arg_traits<false, Args...> \
				, exception_traits<true> \
			{ \
			};\
			template <typename ReturnType, typename ClassType, typename... Args> \
			struct function_type_traits_impl<ReturnType(ClassType::*)(Args..., ...) __VA_ARGS__> \
				: return_traits<ReturnType> \
				, member_fn_traits<ClassType, _const, _volitile, _lref, _rref> \
				, arg_traits<true, Args...> \
				, exception_traits<false> \
			{ \
			};\
			template <typename ReturnType, typename ClassType, typename... Args> \
			struct function_type_traits_impl<ReturnType(ClassType::*)(Args..., ...) __VA_ARGS__ noexcept> \
				: return_traits<ReturnType> \
				, member_fn_traits<ClassType, _const, _volitile, _lref, _rref> \
				, arg_traits<true, Args...> \
				, exception_traits<true> \
			{ \
			};

			MemberFunction(false, false, false, false)
			MemberFunction(true, false, false, false, const)
			MemberFunction(false, true, false, false, volatile)
			MemberFunction(true, true, false, false, const volatile)

			MemberFunction(false, false, true, false, &)
			MemberFunction(true, false, true, false, const&)
			MemberFunction(false, true, true, false, volatile&)
			MemberFunction(true, true, true, false, const volatile&)

			MemberFunction(false, false, false, true, &&)
			MemberFunction(true, false, false, true, const&&)
			MemberFunction(false, true, false, true, volatile&&)
			MemberFunction(true, true, false, true, const volatile&&)

		#undef MemberFunction

	}


	template <typename FnType>
	struct fn_type_traits : detail::function_type_traits_impl<std::remove_cvref_t<std::remove_pointer_t<FnType>>>
	{
		using identity = detail::function_type_traits_impl<std::remove_cvref_t<std::remove_pointer_t<FnType>>>;
	};


	template <auto FnPtr>
	struct fn_traits : fn_type_traits<decltype(FnPtr)>
	{
	};

}

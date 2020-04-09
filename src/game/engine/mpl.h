#ifndef __MPL_H__
#define __MPL_H__

#include <type_traits>
#include <tuple>

namespace mpl {

template <template <typename...> class TNewName, typename T>
struct RenameHelper;

// "Renames" `TOldName<Ts...>` to `TNewName<Ts...>`.
template <template <typename...> class TNewName,
	template <typename...> class TOldName, typename... Ts>
struct RenameHelper<TNewName, TOldName<Ts...>>
{
	using type = TNewName<Ts...>;
};

template <template <typename...> class TNewName, typename T>
using Rename = typename RenameHelper<TNewName, T>::type;

// Compile-time list of types.
template <typename... Ts>
struct TypeList
{
	// Size of the list.
	static constexpr std::size_t size{sizeof...(Ts)};
};

template <typename, typename>
struct IndexOf;

// IndexOf base case: found the type we're looking for.
template <typename T, typename... Ts>
struct IndexOf<T, TypeList<T, Ts...>>
	: std::integral_constant<std::size_t, 0>
{
};

// IndexOf recursive case: 1 + IndexOf the rest of the types.
template <typename T, typename TOther, typename... Ts>
struct IndexOf<T, TypeList<TOther, Ts...>>
	: std::integral_constant<std::size_t,
      1 + IndexOf<T, TypeList<Ts...>>{}>
{
};

// Count base case: 0.
template <typename T, typename TTypeList>
struct CountHelper : std::integral_constant<std::size_t, 0>
{
};

// Interface type alias.
template <typename T, typename TTypeList>
using Count = CountHelper<T, TTypeList>;

// Count recursive case.
template <typename T, typename T0, typename... Ts>
struct CountHelper<T, TypeList<T0, Ts...>>
	: std::integral_constant<std::size_t,
		  (std::is_same<T, T0>{} ? 1 : 0) +
			  Count<T, TypeList<Ts...>>{}>
{
};

// Alias for `Count > 0`.
template <typename T, typename TTypeList>
using Contains =
	std::integral_constant<bool, (Count<T, TTypeList>{} > 0)>;

template <typename TTypeList>
struct allHaveDefaultConstructor {};

template<>
struct allHaveDefaultConstructor<TypeList<>> : std::true_type {};

template<typename T, typename ... Ts>
struct allHaveDefaultConstructor<TypeList<T, Ts...>>
	: std::integral_constant<bool,
		std::is_default_constructible<T>{} &&
			allHaveDefaultConstructor<TypeList<Ts...>>{}>
{
};

template<typename F, typename ... Ts, size_t ... Is>
inline void for_each_in_tuple(std::tuple<Ts...> &tuple, F func, std::index_sequence<Is...>) {
	(func(std::get<Is>(tuple)), ...);
}

template<typename F, typename ... Ts>
inline void for_each_in_tuple(std::tuple<Ts...> &tuple, F func) {
	for_each_in_tuple(tuple, func, std::make_index_sequence<sizeof...(Ts)>());
}

template<typename T, typename L>
struct addToListHelper;

template<typename T, typename ... Ts>
struct addToListHelper<T, TypeList<Ts...>> {
	using type = TypeList<T, Ts...>;
};

template<typename T, typename L>
using addToList = typename addToListHelper<T, L>::type;

}

#endif // __MPL_H__
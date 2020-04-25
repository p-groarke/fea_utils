#pragma once
#include <type_traits>

#define FEA_ENABLE_BITMASK_OPERATORS(x) \
	template <> \
	struct has_bitmask_operators<x> : std::true_type { \
		static_assert(std::is_enum_v<x> && !std::is_convertible_v<x, int>, \
				"template parameter is not an enum class type"); \
	}

namespace fea {
template <class T>
struct has_bitmask_operators : std::false_type {};

template <class T>
inline constexpr bool has_bitmask_operators_v = has_bitmask_operators<T>::value;


template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators_v<T>, T>
operator|(T lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	return static_cast<T>(
			static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators_v<T>, T>
operator&(T lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	return static_cast<T>(
			static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators_v<T>, T>
operator^(T lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	return static_cast<T>(
			static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators_v<T>, T>
operator~(T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	return static_cast<T>(~static_cast<underlying>(rhs));
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators_v<T>, T&>
operator|=(T& lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	lhs = static_cast<T>(
			static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
	return lhs;
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators_v<T>, T&>
operator&=(T& lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	lhs = static_cast<T>(
			static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
	return lhs;
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators_v<T>, T&>
operator^=(T& lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	lhs = static_cast<T>(
			static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
	return lhs;
}

/* Helper */
template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators_v<T>,
		std::underlying_type_t<T>>
bitmask_cast(T e) {
	return static_cast<std::underlying_type_t<T>>(e);
}
} // namespace fea

#pragma once
#include <compare>    // operator<=>
#include <functional> // std::hash
#include <memory>     // std::construct_at, std::destroy_at, std::addressof
#include <type_traits>
#include <typeinfo>
#include <utility>
#include "NullableException.h"

namespace SysType {
	template <typename T>
	class Nullable {
	private:
		union {
			T    value;
			char dummy = 0;
		};
		bool hasValue = false;

		template <typename U> struct IsNullable              : std::false_type {};
		template <typename U> struct IsNullable<Nullable<U>> : std::true_type  {};

		constexpr void destroy() noexcept {
			if (hasValue) {
				std::destroy_at(std::addressof(value));
				hasValue = false;
			}
		}

	public:
		// Constructors
		constexpr Nullable() noexcept : dummy(0), hasValue(false) {}
		constexpr Nullable(std::nullptr_t) noexcept : dummy(0), hasValue(false) {}

		template <typename... Args>
		explicit constexpr Nullable(std::in_place_t, Args&&... args) : dummy(0), hasValue(true) {
			std::construct_at(std::addressof(value), std::forward<Args>(args)...);
		}

		template <typename U, typename... Args>
		explicit constexpr Nullable(std::in_place_t, std::initializer_list<U> il, Args&&... args) : dummy(0), hasValue(true) {
			std::construct_at(std::addressof(value), il, std::forward<Args>(args)...);
		}

		constexpr Nullable(const T& val) : dummy(0), hasValue(true) {
			std::construct_at(std::addressof(value), val);
		}

		constexpr Nullable(T&& val) noexcept(std::is_nothrow_move_constructible_v<T>) : dummy(0), hasValue(true) {
			std::construct_at(std::addressof(value), std::move(val));
		}

		Nullable(const Nullable&) requires std::is_trivially_copy_constructible_v<T> = default;
		constexpr Nullable(const Nullable& other) : dummy(0), hasValue(other.hasValue) {
			if (hasValue) std::construct_at(std::addressof(value), other.value);
		}

		Nullable(Nullable&&) requires std::is_trivially_move_constructible_v<T> = default;
		constexpr Nullable(Nullable&& other) noexcept(std::is_nothrow_move_constructible_v<T>) : dummy(0), hasValue(other.hasValue) {
			if (hasValue) {
				std::construct_at(std::addressof(value), std::move(other.value));
				other.destroy();
			}
		}

		// Destructor
		~Nullable() requires std::is_trivially_destructible_v<T> = default;
		constexpr ~Nullable() { destroy(); }

		// Assignment
		Nullable& operator=(const Nullable&) requires std::is_trivially_copyable_v<T> = default;
		constexpr Nullable& operator=(const Nullable& other) {
			if (this == &other) {
				return *this;
			}
			if (hasValue && other.hasValue) {
				value = other.value;
			}
			else if (other.hasValue) {
				std::construct_at(std::addressof(value), other.value);
				hasValue = true;
			}
			else {
				destroy();
			}
			return *this;
		}

		Nullable& operator=(Nullable&&) requires (std::is_trivially_move_assignable_v<T> && std::is_trivially_destructible_v<T>) = default;
		constexpr Nullable& operator=(Nullable&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
			if (this == &other) {
				return *this;
			}
			if (hasValue && other.hasValue) {
				value = std::move(other.value);
				other.destroy();
			}
			else if (other.hasValue) {
				std::construct_at(std::addressof(value), std::move(other.value));
				hasValue = true;
				other.destroy();
			}
			else {
				destroy();
			}
			return *this;
		}

		constexpr Nullable& operator=(const T& rhs) {
			if (hasValue) {
				value = rhs;
			}
			else {
				std::construct_at(std::addressof(value), rhs);
				hasValue = true;
			}
			return *this;
		}

		constexpr Nullable& operator=(T&& rhs) noexcept(std::is_nothrow_move_assignable_v<T>) {
			if (hasValue) {
				value = std::move(rhs);
			}
			else {
				std::construct_at(std::addressof(value), std::move(rhs));
				hasValue = true;
			}
			return *this;
		}

		constexpr Nullable& operator=(std::nullptr_t) noexcept { 
			destroy(); 
			return *this; 
		}

		// Accessors
		[[nodiscard]] constexpr bool HasValue() const noexcept { 
			return hasValue; 
		}

		[[nodiscard]] constexpr T& Value() {
			if (hasValue) return value;
			throw NullableException(typeid(T));
		}

		[[nodiscard]] constexpr const T& Value() const {
			if (hasValue) return value;
			throw NullableException(typeid(T));
		}

		[[nodiscard]] constexpr T ValueOr(const T& fallback) const {
			return hasValue ? value : fallback;
		}

		[[nodiscard]] constexpr T ValueOr(T&& fallback) const {
			return hasValue ? value : std::move(fallback);
		}

		// Mutation
		template <typename... Args> constexpr T& Emplace(Args&&... args) {
			destroy();
			std::construct_at(std::addressof(value), std::forward<Args>(args)...);
			hasValue = true;
			return value;
		}

		template <typename U, typename... Args>
		constexpr T& Emplace(std::initializer_list<U> il, Args&&... args) {
			destroy();
			std::construct_at(std::addressof(value), il, std::forward<Args>(args)...);
			hasValue = true;
			return value;
		}

		constexpr void Swap(Nullable& other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_swappable_v<T>) {
			if (hasValue && other.hasValue) {
				std::swap(value, other.value);
			}
			else if (hasValue) {
				std::construct_at(std::addressof(other.value), std::move(value));
				other.hasValue = true;
				destroy();
			}
			else if (other.hasValue) {
				std::construct_at(std::addressof(value), std::move(other.value));
				hasValue = true;
				other.destroy();
			}
		}

		// Pointer-like access — unchecked, UB if null. Use Value() for safe access.
		constexpr T* operator->() noexcept {
			return std::addressof(value);
		}
		constexpr const T* operator->() const noexcept {
			return std::addressof(value);
		}
		constexpr T& operator*() noexcept {
			return value;
		}
		constexpr const T& operator*() const noexcept {
			return value;
		}

		// Monadic
		template <typename F> [[nodiscard]] constexpr auto Transform(F&& func) const -> Nullable<std::invoke_result_t<F, const T&>> {
			using U = std::invoke_result_t<F, const T&>;
			if (hasValue) {
				return Nullable<U>(std::forward<F>(func)(value));
			}
			return Nullable<U>{};
		}

		template <typename F> [[nodiscard]] constexpr auto AndThen(F&& func) const -> std::invoke_result_t<F, const T&> {
			using Result = std::invoke_result_t<F, const T&>;
			static_assert(IsNullable<Result>::value, "AndThen: F must return a Nullable<U>");
			if (hasValue) {
				return std::forward<F>(func)(value);
			}
			return Result{};
		}

		template <typename F> [[nodiscard]] constexpr Nullable OrElse(F&& func) const {
			if (!hasValue) {
				return std::forward<F>(func)();
			}
			return *this;
		}

		// Conversions
		constexpr explicit operator bool() const noexcept {
			return hasValue;
		}
		constexpr explicit operator T() const {
			return Value();
		}

		// Equality
		constexpr bool operator==(std::nullptr_t) const noexcept {
			return !hasValue;
		}
		constexpr bool operator!=(std::nullptr_t) const noexcept {
			return hasValue;
		}
		constexpr bool operator==(const T& obj) const {
			return hasValue && value == obj;
		}
		constexpr bool operator!=(const T& obj) const {
			return !(*this == obj);
		}

		constexpr bool operator==(const Nullable& other) const {
			if (!hasValue && !other.hasValue) {
				return true;
			}
			if (hasValue != other.hasValue) {
				return false;
			}
			return value == other.value;
		}
		constexpr bool operator!=(const Nullable& other) const { return !(*this == other); }

		// Ordering — operator<=> generates <, <=, >, >= for all pairs
		[[nodiscard]] constexpr std::strong_ordering operator<=>(std::nullptr_t) const noexcept {
			if (hasValue) {
				return std::strong_ordering::greater;
			}
			return std::strong_ordering::equal;
		}

		[[nodiscard]] constexpr auto operator<=>(const Nullable& other) const
			requires std::three_way_comparable<T>
			-> std::compare_three_way_result_t<T>
		{
			if (!hasValue && !other.hasValue) {
				return std::strong_ordering::equal;
			}
			if (!hasValue) {
				return std::strong_ordering::less;
			}
			if (!other.hasValue) {
				return std::strong_ordering::greater;
			}
			return value <=> other.value;
		}

		[[nodiscard]] constexpr auto operator<=>(const T& rhs) const
			requires std::three_way_comparable<T>
			-> std::compare_three_way_result_t<T>
		{
			if (!hasValue) {
				return std::strong_ordering::less;
			}
			return value <=> rhs;
		}

		friend constexpr auto operator<=>(const T& lhs, const Nullable& rhs)
			requires std::three_way_comparable<T>
			-> std::compare_three_way_result_t<T>
		{
			if (!rhs.hasValue) {
				return std::strong_ordering::greater;
			}
			return lhs <=> rhs.value;
		}
	};

	template <typename T>
	Nullable(T) -> Nullable<T>;

	template <typename T>
	constexpr void swap(Nullable<T>& a, Nullable<T>& b) noexcept(noexcept(a.Swap(b))) {
		a.Swap(b);
	}
}

namespace std {
	template <typename T> struct hash<SysType::Nullable<T>> {
		size_t operator()(const SysType::Nullable<T>& n) const noexcept {
			return n.HasValue() ? hash<T>{}(*n) : 0;
		}
	};
}

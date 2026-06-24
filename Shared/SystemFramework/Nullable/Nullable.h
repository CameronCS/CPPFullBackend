#pragma once
#include <stdexcept>
#include <typeinfo>
#include <utility>

namespace SysType {
	template <typename T>
	class Nullable {
	private:
		T object{};
		bool hasValue = false;
	public:
		Nullable() = default;
		Nullable(const T& value) : object(value), hasValue(true) {}
		Nullable(T&& value) : object(std::move(value)), hasValue(true) {}
		Nullable(std::nullptr_t) : hasValue(false) {}

		Nullable<T>& operator=(const T& rhs) {
			object = rhs;
			hasValue = true;
			return *this;
		}

		Nullable<T>& operator=(T&& rhs) {
			object = std::move(rhs);
			hasValue = true;
			return *this;
		}

		Nullable<T>& operator=(std::nullptr_t) {
			object = T{};  
			hasValue = false;
			return *this;
		}

		bool HasValue() const {
			return hasValue;
		}

		T& Value() {
			if (hasValue) return object;
			throw std::runtime_error(
				std::string("Nullable type of ") + typeid(T).name() + " is null"
			);
		}

		const T& Value() const {
			if (hasValue) return object;
			throw std::runtime_error(
				std::string("Nullable type of ") + typeid(T).name() + " is null"
			);
		}

		T ValueOr(const T& fallback) const {
			return hasValue ? object : fallback;
		}

		explicit operator bool() const {
			return hasValue;
		}

		explicit operator T() const {
			return Value();
		}

		bool operator==(std::nullptr_t) const {
			return !hasValue;
		}

		bool operator!=(std::nullptr_t) const {
			return hasValue;
		}

		bool operator==(const T& obj) const {
			return hasValue && object == obj;
		}

		bool operator!=(const T& obj) const {
			return !(*this == obj);
		}

		bool operator==(const Nullable<T>& other) const {
			if (!hasValue && !other.hasValue)
			{
				return true;
			}
			if (hasValue != other.hasValue) 
			{
				return false;
			}

			return object == other.object;
		}

		bool operator!=(const Nullable<T>& other) const {
			return !(*this == other);
		}
	};
}
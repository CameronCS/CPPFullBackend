#pragma once
#include <stdexcept>
#include <string>
#include <typeinfo>

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#endif

namespace SysType {
	class NullableException : public std::exception {
	private:
		std::string message;

		static std::string Demangle(const char* name) {
#ifdef __GNUG__
			int status = 0;
			char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
			std::string result = (status == 0 ? demangled : name);
			free(demangled);
			return result;
#else
			return name;
#endif
		}

	public:
		explicit NullableException(const std::type_info& type)
			: message(std::string("Nullable<") + Demangle(type.name()) + "> is null") {}

		const char* what() const noexcept override {
			return message.c_str();
		}
	};
}

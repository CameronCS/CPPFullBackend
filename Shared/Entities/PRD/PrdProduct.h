#pragma once

#include <string>

namespace Entities {
	struct PRDProduct {
		int ID = 0;
		std::string Name = "";
		double Price = 0.0;

		PRDProduct(int id, const std::string& name, double price)
			: ID(id), Name(name), Price(price) {}

		PRDProduct(const std::string& name, double price)
			: ID(0), Name(name), Price(price) {}

		PRDProduct() = default;
	};
}

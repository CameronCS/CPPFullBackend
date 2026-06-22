#ifndef _____PRODUCT_H_____
#define _____PRODUCT_H_____

#include <string>
#include <nlohmann/json.hpp>

namespace Models {
	class Product {
	public:
		int Id = 0;
		std::string Name = "";
		double Price = 0;

		Product() {
			this->Id = 0;
			this->Name = "";
			this->Price = 0;
		}

		Product(int id, const std::string& name, double price) {
			this->Id = id;
			this->Name = name;
			this->Price = price;
		}
	};

	inline void to_json(nlohmann::json& j, const Product& p) {
		j["id"] = p.Id;
		j["name"] = p.Name;
		j["price"] = p.Price;
	}

	inline void from_json(const nlohmann::json& j, Product& p) {
		if (j.contains("id"))    p.Id = j["id"].get<int>();
		if (j.contains("name"))  p.Name = j["name"].get<std::string>();
		if (j.contains("price")) p.Price = j["price"].get<double>();
	}
}

#endif
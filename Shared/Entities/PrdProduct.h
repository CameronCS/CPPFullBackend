#ifndef __PRD_Product_H__
#define __PRD_Product_H__

#include <string>

namespace Entities {
	class PRDProduct {
	public:
		int ID = 0;
		std::string Name = "";
		double Price = 0;

		PRDProduct(int id, std::string name, double price) {
			this->ID = id;
			this->Name = name;
			this->Price = price;
		}

		PRDProduct(std::string name, double price) {
			this->Name = name;
			this->Price = price;
		}

		PRDProduct() {
			this->ID = 0;
			this->Name = "";
			this->Price = 0;
		}
	};
}
#endif
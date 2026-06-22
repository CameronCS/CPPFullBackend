#pragma once

#include <vector>
#include <optional>
#include <Entities.h>

namespace DataService::Interface {
	class IProductRepository {
	public:
		virtual ~IProductRepository() = default;
		[[nodiscard]] virtual bool WriteProductToDB(const Entities::PRDProduct&) = 0;
		[[nodiscard]] virtual std::optional<Entities::PRDProduct> GetProductById(int id) = 0;
		[[nodiscard]] virtual std::vector<Entities::PRDProduct> GetAllProducts() = 0;
		[[nodiscard]] virtual bool DeleteProduct(int productId) = 0;
		[[nodiscard]] virtual bool UpdateProduct(const Entities::PRDProduct& product) = 0;
	};
}

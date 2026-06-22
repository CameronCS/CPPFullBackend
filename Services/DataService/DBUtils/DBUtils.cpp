#include "DBUtils.h"

std::optional<Entities::PRDProduct> DataService::Utils::CreateProductFromResult(nanodbc::result& ndbcRes) {
	try {
		Entities::PRDProduct product;
		product.ID = ndbcRes.get<int>("ID");
		product.Name = ndbcRes.get<std::string>("Name");
		product.Price = ndbcRes.get<double>("Price");
		return product;
	}
	catch (const nanodbc::index_range_error&) {
		return std::nullopt;
	}
}
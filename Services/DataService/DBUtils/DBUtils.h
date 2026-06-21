#include "../ProductRepository.h"
#include <nanodbc/nanodbc.h>
#include <Entities.h>

namespace DataService::Utils {
	std::optional<Entities::PRDProduct> CreateProductFromResult(nanodbc::result ndbcRes);
}
#pragma once

#include <nanodbc/nanodbc.h>
#include <Entities.h>
#include <optional>

namespace DataService::Utils {
	std::optional<Entities::PRDProduct> CreateProductFromResult(nanodbc::result ndbcRes);
}
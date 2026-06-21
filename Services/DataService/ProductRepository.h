#ifndef __PDS_H__
#define __PDS_H__

#include <iostream>
#include <vector>
#include <optional>
#include <Entities.h>
#include <IProductRepository.h>

namespace DataService {
	class ProductRepository : DataService::Interface::IProductRepository {
		virtual bool WriteProductToDB(Entities::PRDProduct);
		virtual std::optional<Entities::PRDProduct> GetProductById(int id);
		virtual std::vector<Entities::PRDProduct> GetAllProducts();
		virtual bool DeleteProduct(int productId);
		virtual bool UpdateProduct(Entities::PRDProduct product);
	};
}

#endif
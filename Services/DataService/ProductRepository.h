#ifndef __PDS_H__
#define __PDS_H__

#include <iostream>
#include <vector>
#include <optional>
#include <Entities.h>
#include <IProductRepository.h>

namespace DataService {
	class ProductRepository : public DataService::Interface::IProductRepository {
	public:
		virtual bool WriteProductToDB(Entities::PRDProduct) override;
		virtual std::optional<Entities::PRDProduct> GetProductById(int id) override;
		virtual std::vector<Entities::PRDProduct> GetAllProducts() override;
		virtual bool DeleteProduct(int productId) override;
		virtual bool UpdateProduct(Entities::PRDProduct product) override;
	};
}

#endif
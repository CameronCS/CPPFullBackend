#ifndef _____PRODUCT_SERVICE_H_____
#define _____PRODUCT_SERVICE_H_____

#include <IProductService.h>
#include <Models.h>
#include <optional>
#include <vector>
#include <Mapping/Mapping.h>

namespace BusinessService {
	class ProductService : public BusinessService::Interface::IProductService {
	private:
		using Repo = DataService::Interface::IProductRepository;
		using Logger = SystemFramework::Logging::ILogger;
		using Mapper = SystemFramework::Mapping::Mapper;
	public:
		ProductService(Repo* productRepository, Logger* logger, Mapper* mapper) : IProductService(productRepository, logger, mapper) {}

		bool ProcessProduct(Models::Product product) override;
		std::optional<Models::Product> GetProductById(int id) override;
		std::vector<Models::Product> GetAllProducts() override;
		bool DeleteProduct(int productId) override;
		bool UpdateProduct(Models::Product productToUpdate) override;
	};
}
#endif

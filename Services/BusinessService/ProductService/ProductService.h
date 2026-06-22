#pragma once

#include <IProductService.h>
#include <IProductRepository.h>
#include <Logging/ILogger.h>
#include <Mapping/Mapping.h>
#include <Models.h>
#include <optional>
#include <vector>

namespace BusinessService {
	class ProductService : public BusinessService::Interface::IProductService {
	public:
		ProductService(DataService::Interface::IProductRepository* productRepository, SystemFramework::Logging::ILogger* logger, SystemFramework::Mapping::Mapper* mapper)
			: _productRepository(productRepository), _logger(logger), _mapper(mapper) {
			_logger->SetServiceLevel("Business", "Product Service");
		}
		bool ProcessProduct(const Models::Product& product) override;
		std::optional<Models::Product> GetProductById(int id) override;
		std::vector<Models::Product> GetAllProducts() override;
		bool DeleteProduct(int productId) override;
		bool UpdateProduct(const Models::Product& productToUpdate) override;
	private:
		DataService::Interface::IProductRepository* _productRepository;
		SystemFramework::Logging::ILogger* _logger;
		SystemFramework::Mapping::Mapper* _mapper;
	};
}

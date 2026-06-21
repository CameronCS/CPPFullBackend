#ifndef _____IPRODUCT_SERVICE_H_____
#define _____IPRODUCT_SERVICE_H_____

#include <Models.h>
#include <IProductRepository.h>
#include <Logging/ILogger.h>
#include <optional>
#include <vector>

namespace BusinessService::Interface {
    class IProductService {
    public:
        IProductService(DataService::Interface::IProductRepository* productRepository, SystemFramework::Logging::ILogger* logger) {
            _productRepository = productRepository;
            _logger = logger;
            _logger->SetServiceLevel("Business", "Product Service");
        }
        virtual bool ProcessProduct(Models::Product product) = 0;
        virtual std::optional<Models::Product> GetProductById(int id) = 0;
        virtual std::vector<Models::Product> GetAllProducts() = 0;
        virtual bool DeleteProduct(int productId) = 0;
        virtual bool UpdateProduct(Models::Product productToUpdate) = 0;
        virtual ~IProductService() = default;
    protected:
        DataService::Interface::IProductRepository* _productRepository;
        SystemFramework::Logging::ILogger* _logger;
    };
}

#endif
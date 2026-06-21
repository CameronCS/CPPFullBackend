#ifndef _____PRODUCT_SERVICE_H_____
#define _____PRODUCT_SERVICE_H_____

#include <IProductService.h>
#include <Models.h>
#include <optional>
#include <vector>

namespace BusinessService {
    class ProductService : public BusinessService::Interface::IProductService {
    public:
        ProductService(DataService::Interface::IProductRepository* productRepository, SystemFramework::Logging::ILogger* logger)
            : IProductService(productRepository, logger) {}

        bool ProcessProduct(Models::Product product) override;
        std::optional<Models::Product> GetProductById(int id) override;
        std::vector<Models::Product> GetAllProducts() override;
        bool DeleteProduct(int productId) override;
        bool UpdateProduct(Models::Product productToUpdate) override;
    };
}
#endif

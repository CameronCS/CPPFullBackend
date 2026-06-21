#ifndef _____PRODUCT_SERVICE_H_____
#define _____PRODUCT_SERVICE_H_____

#include <IProductService.h>
#include <IProductRepository.h>
#include <Models.h>
#include <optional>
#include <vector>

namespace BusinessService {
    class ProductService : public BusinessService::Interface::IProductService {
    public:
        ProductService(DataService::Interface::IProductRepository* productRepository)
        {
            this->_productRepository = productRepository;
        }

        virtual bool ProcessProduct(Models::Product product) override;
        virtual std::optional<Models::Product> GetProductById(int id) override;
        virtual std::vector<Models::Product> GetAllProducts() override;
        virtual bool DeleteProduct(int productId) override;
        virtual bool UpdateProduct(Models::Product productToUpdate) override;
    private:
        DataService::Interface::IProductRepository* _productRepository;
    };
}
#endif
#ifndef _____IPRODUCT_SERVICE_H_____
#define _____IPRODUCT_SERVICE_H_____

#include <Models.h>
#include <optional>
#include <vector>

namespace BusinessService::Interface {
    class IProductService {
    public:
        virtual bool ProcessProduct(Models::Product product) = 0;
        virtual std::optional<Models::Product> GetProductById(int id) = 0;
        virtual std::vector<Models::Product> GetAllProducts() = 0;
        virtual bool DeleteProduct(int productId) = 0;
        virtual bool UpdateProduct(Models::Product productToUpdate) = 0;
        virtual ~IProductService() = default;
    };
}

#endif
#pragma once

#include <Models.h>
#include <optional>
#include <vector>

namespace BusinessService::Interface {
    class IProductService {
    public:
        virtual ~IProductService() = default;
        [[nodiscard]] virtual bool ProcessProduct(const Models::Product& product) = 0;
        [[nodiscard]] virtual std::optional<Models::Product> GetProductById(int id) = 0;
        [[nodiscard]] virtual std::vector<Models::Product> GetAllProducts() = 0;
        [[nodiscard]] virtual bool DeleteProduct(int productId) = 0;
        [[nodiscard]] virtual bool UpdateProduct(const Models::Product& productToUpdate) = 0;
    };
}

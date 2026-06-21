#include "ProductService.h"
#include <Models.h>

bool BusinessService::ProductService::ProcessProduct(Models::Product newProduct) {
	Entities::PRDProduct dbProduct(newProduct.Name, newProduct.Price);
	bool hasWrittenProduct = this->_productRepository->WriteProductToDB(dbProduct);
	return hasWrittenProduct;
}

std::optional<Models::Product> BusinessService::ProductService::GetProductById(int id) {
	std::optional<Entities::PRDProduct> product = this->_productRepository->GetProductById(id);
	if (product.has_value()) {
		Entities::PRDProduct raw = product.value();
		Models::Product result(raw.ID, raw.Name, raw.Price);
		return result;
	}
	return std::nullopt;
}

std::vector<Models::Product> BusinessService::ProductService::GetAllProducts() {
	std::vector<Entities::PRDProduct> dbRows = this->_productRepository->GetAllProducts();
	std::vector<Models::Product> products;
	for (const Entities::PRDProduct& dbProd : dbRows) {
		Models::Product product(dbProd.ID, dbProd.Name, dbProd.Price);
		products.push_back(product);
	}
	return products;
}

bool BusinessService::ProductService::DeleteProduct(int productId) { // I know I can 1 line it but I like verbosity
	bool isDeleted = this->_productRepository->DeleteProduct(productId);
	return isDeleted;
}

bool BusinessService::ProductService::UpdateProduct(Models::Product updateProduct) { // I know I can 1 line it but I like verbosity
	Entities::PRDProduct rawProduct(updateProduct.Id, updateProduct.Name, updateProduct.Price);
	bool result = this->_productRepository->UpdateProduct(rawProduct);
	return result;
}

#include "ProductService.h"
#include <Models.h>

bool BusinessService::ProductService::ProcessProduct(Models::Product newProduct) {
	if (newProduct.Name.empty()) {
		_logger->LogError("ProcessProduct - Product name cannot be empty");
		return false;
	}
	if (newProduct.Price <= 0) {
		_logger->LogError("ProcessProduct - Product price must be greater than zero");
		return false;
	}
	try {
		Entities::PRDProduct dbProduct = _mapper->Map<Models::Product, Entities::PRDProduct>(newProduct);
		bool hasWrittenProduct = this->_productRepository->WriteProductToDB(dbProduct);
		if (!hasWrittenProduct) {
			_logger->LogWarning("ProcessProduct - Repository returned false for product: " + newProduct.Name);
		}
		return hasWrittenProduct;
	}
	catch (const std::exception& e) {
		_logger->LogError("ProcessProduct - Unexpected error: " + std::string(e.what()));
		return false;
	}
}

std::optional<Models::Product> BusinessService::ProductService::GetProductById(int id) {
	if (id <= 0) {
		_logger->LogError("GetProductById - Id must be greater than zero, received: " + std::to_string(id));
		return std::nullopt;
	}
	try {
		std::optional<Entities::PRDProduct> product = this->_productRepository->GetProductById(id);
		if (product.has_value()) {
			Entities::PRDProduct raw = product.value();
			Models::Product result = _mapper->Map<Entities::PRDProduct, Models::Product>(raw);
			return result;
		}
		return std::nullopt;
	}
	catch (const std::exception& e) {
		_logger->LogError("GetProductById - Unexpected error for id " + std::to_string(id) + ": " + std::string(e.what()));
		return std::nullopt;
	}
}

std::vector<Models::Product> BusinessService::ProductService::GetAllProducts() {
	try {
		std::vector<Entities::PRDProduct> dbRows = this->_productRepository->GetAllProducts();
		std::vector<Models::Product> products;
		for (const Entities::PRDProduct& dbProd : dbRows) {
			Models::Product product = _mapper->Map<Entities::PRDProduct, Models::Product>(dbProd);
			products.push_back(product);
		}
		return products;
	}
	catch (const std::exception& e) {
		_logger->LogError("GetAllProducts - Unexpected error: " + std::string(e.what()));
		return {};
	}
}

bool BusinessService::ProductService::DeleteProduct(int productId) {
	if (productId <= 0) {
		_logger->LogError("DeleteProduct - Id must be greater than zero, received: " + std::to_string(productId));
		return false;
	}
	try {
		bool isDeleted = this->_productRepository->DeleteProduct(productId);
		if (!isDeleted) {
			_logger->LogWarning("DeleteProduct - Repository returned false for id: " + std::to_string(productId));
		}
		return isDeleted;
	}
	catch (const std::exception& e) {
		_logger->LogError("DeleteProduct - Unexpected error for id " + std::to_string(productId) + ": " + std::string(e.what()));
		return false;
	}
}

bool BusinessService::ProductService::UpdateProduct(Models::Product updateProduct) {
	if (updateProduct.Id <= 0) {
		_logger->LogError("UpdateProduct - Id must be greater than zero, received: " + std::to_string(updateProduct.Id));
		return false;
	}
	if (updateProduct.Name.empty()) {
		_logger->LogError("UpdateProduct - Product name cannot be empty");
		return false;
	}
	if (updateProduct.Price <= 0) {
		_logger->LogError("UpdateProduct - Product price must be greater than zero");
		return false;
	}
	try {
		Entities::PRDProduct rawProduct = _mapper->Map<Models::Product, Entities::PRDProduct>(updateProduct);
		bool result = this->_productRepository->UpdateProduct(rawProduct);
		if (!result) {
			_logger->LogWarning("UpdateProduct - Repository returned false for product id: " + std::to_string(updateProduct.Id));
		}
		return result;
	}
	catch (const std::exception& e) {
		_logger->LogError("UpdateProduct - Unexpected error for id " + std::to_string(updateProduct.Id) + ": " + std::string(e.what()));
		return false;
	}
}

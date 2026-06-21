#include "ProductGateway.h"
#include <IProductService.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

void APIGateway::ProductGateway::AddProduct(const httplib::Request& request, httplib::Response& response) {
	_logger->Log("POST /product - Add product request received");
	try {
		if (request.body.empty()) {
			_logger->LogError("POST /product - Request body is empty");
			response.status = httplib::BadRequest_400;
			return;
		}
		nlohmann::json json = nlohmann::json::parse(request.body);
		std::optional<Models::Product> product = this->ToObject<Models::Product>(json);
		if (!product.has_value()) {
			_logger->LogError("POST /product - Failed to deserialize product from body");
			response.status = httplib::BadRequest_400;
			return;
		}
		auto scope = _container->CreateScope();
		auto* service = scope.Resolve<BusinessService::Interface::IProductService>();
		service->ProcessProduct(product.value());
		_logger->Log("POST /product - Product created successfully");
		response.status = httplib::Created_201;
	}
	catch (const nlohmann::json::parse_error& e) {
		_logger->LogError("POST /product - Invalid JSON: " + std::string(e.what()));
		response.status = httplib::BadRequest_400;
	}
	catch (const std::exception& e) {
		_logger->LogError("POST /product - Unexpected error: " + std::string(e.what()));
		response.status = httplib::InternalServerError_500;
	}
}

void APIGateway::ProductGateway::GetProductById(const httplib::Request& request, httplib::Response& response) {
	std::string idStr = request.path_params.at("id");
	_logger->Log("GET /product/" + idStr + " - Get product by id request received");
	try {
		int id = std::stoi(idStr);
		auto scope = _container->CreateScope();
		auto* service = scope.Resolve<BusinessService::Interface::IProductService>();
		std::optional<Models::Product> product = service->GetProductById(id);
		if (!product.has_value()) {
			_logger->LogWarning("GET /product/" + idStr + " - Product not found");
			response.status = httplib::NotFound_404;
			return;
		}
		_logger->Log("GET /product/" + idStr + " - Product found");
		response.status = httplib::OK_200;
		response.set_content(this->ToJson(product.value()), this->json_content_type);
	}
	catch (const std::invalid_argument&) {
		_logger->LogError("GET /product/" + idStr + " - Invalid id format");
		response.status = httplib::BadRequest_400;
	}
	catch (const std::out_of_range&) {
		_logger->LogError("GET /product/" + idStr + " - Id out of range");
		response.status = httplib::BadRequest_400;
	}
	catch (const std::exception& e) {
		_logger->LogError("GET /product/" + idStr + " - Unexpected error: " + std::string(e.what()));
		response.status = httplib::InternalServerError_500;
	}
}

void APIGateway::ProductGateway::GetAllProducts(const httplib::Request& request, httplib::Response& response) {
	_logger->Log("GET /products - Get all products request received");
	try {
		auto scope = _container->CreateScope();
		auto* service = scope.Resolve<BusinessService::Interface::IProductService>();
		std::vector<Models::Product> products = service->GetAllProducts();
		_logger->Log("GET /products - Returning " + std::to_string(products.size()) + " product(s)");
		if (products.empty()) {
			response.status = httplib::OK_200;
			response.set_content("[]", this->json_content_type);
			return;
		}
		response.status = httplib::OK_200;
		response.set_content(this->ToJson(products), this->json_content_type);
	}
	catch (const std::exception& e) {
		_logger->LogError("GET /products - Unexpected error: " + std::string(e.what()));
		response.status = httplib::InternalServerError_500;
	}
}

void APIGateway::ProductGateway::DeleteProduct(const httplib::Request& request, httplib::Response& response) {
	std::string idStr = request.path_params.at("id");
	_logger->Log("DELETE /product/" + idStr + " - Delete product request received");
	try {
		int id = std::stoi(idStr);
		auto scope = _container->CreateScope();
		auto* service = scope.Resolve<BusinessService::Interface::IProductService>();
		bool isDeleted = service->DeleteProduct(id);
		if (isDeleted) {
			_logger->Log("DELETE /product/" + idStr + " - Product deleted");
			response.status = httplib::OK_200;
		}
		else {
			_logger->LogWarning("DELETE /product/" + idStr + " - Product not found");
			response.status = httplib::NotFound_404;
		}
	}
	catch (const std::invalid_argument&) {
		_logger->LogError("DELETE /product/" + idStr + " - Invalid id format");
		response.status = httplib::BadRequest_400;
	}
	catch (const std::out_of_range&) {
		_logger->LogError("DELETE /product/" + idStr + " - Id out of range");
		response.status = httplib::BadRequest_400;
	}
	catch (const std::exception& e) {
		_logger->LogError("DELETE /product/" + idStr + " - Unexpected error: " + std::string(e.what()));
		response.status = httplib::InternalServerError_500;
	}
}

void APIGateway::ProductGateway::UpdateProduct(const httplib::Request& request, httplib::Response& response) {
	std::string idStr = request.path_params.at("id");
	_logger->Log("PUT /product/" + idStr + " - Update product request received");
	try {
		int id = std::stoi(idStr);
		if (request.body.empty()) {
			_logger->LogError("PUT /product/" + idStr + " - Request body is empty");
			response.status = httplib::BadRequest_400;
			return;
		}
		nlohmann::json postObject = nlohmann::json::parse(request.body);
		std::optional<Models::Product> product = this->ToObject<Models::Product>(postObject);
		if (!product.has_value()) {
			_logger->LogError("PUT /product/" + idStr + " - Failed to deserialize product from body");
			response.status = httplib::BadRequest_400;
			return;
		}
		Models::Product updatedProduct = product.value();
		if (id != updatedProduct.Id) {
			_logger->LogError("PUT /product/" + idStr + " - Id mismatch: path id does not match body id");
			response.status = httplib::NotAcceptable_406;
			return;
		}
		auto scope = _container->CreateScope();
		auto* service = scope.Resolve<BusinessService::Interface::IProductService>();
		bool isUpdated = service->UpdateProduct(updatedProduct);
		if (isUpdated) {
			_logger->Log("PUT /product/" + idStr + " - Product updated");
			response.status = httplib::OK_200;
			return;
		}
		_logger->LogWarning("PUT /product/" + idStr + " - Product not found");
		response.status = httplib::NotFound_404;
	}
	catch (const nlohmann::json::parse_error& e) {
		_logger->LogError("PUT /product/" + idStr + " - Invalid JSON: " + std::string(e.what()));
		response.status = httplib::BadRequest_400;
	}
	catch (const std::invalid_argument&) {
		_logger->LogError("PUT /product/" + idStr + " - Invalid id format");
		response.status = httplib::BadRequest_400;
	}
	catch (const std::out_of_range&) {
		_logger->LogError("PUT /product/" + idStr + " - Id out of range");
		response.status = httplib::BadRequest_400;
	}
	catch (const std::exception& e) {
		_logger->LogError("PUT /product/" + idStr + " - Unexpected error: " + std::string(e.what()));
		response.status = httplib::InternalServerError_500;
	}
}

void APIGateway::ProductGateway::MapMethods(httplib::SSLServer& server) {
	server.Get("/products", [this](const httplib::Request& request, httplib::Response& response) { GetAllProducts(request, response); });
	server.Get("/product/:id", [this](const httplib::Request& request, httplib::Response& response) { GetProductById(request, response); });
	server.Post("/product", [this](const httplib::Request& request, httplib::Response& response) { AddProduct(request, response); });
	server.Delete("/product/:id", [this](const httplib::Request& request, httplib::Response& response) { DeleteProduct(request, response); });
	server.Put("/product/:id", [this](const httplib::Request& request, httplib::Response& response) { UpdateProduct(request, response); });
}

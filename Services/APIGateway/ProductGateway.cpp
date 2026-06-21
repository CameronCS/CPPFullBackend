#include "ProductGateway.h"
#include <IProductService.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

constexpr const char* json_content_type = "application/json";

template<typename T> static std::string ToJson(T object) {
	try {
		nlohmann::json j = object;
		return j.dump();
	}
	catch (nlohmann::json::type_error& e) {
		std::cout << "Json Error: " << e.what() << std::endl;
		return "";
	}
}

template <typename T> std::optional<T> ToObject(nlohmann::json json) {
	try {
		T jObj = json.get<T>();
		return jObj;
	}
	catch (const nlohmann::json::exception& e) {  // base class for all nlohmann exceptions
		std::cout << "Could Not Make Object: " << e.what() << std::endl;
		return std::nullopt;
	}
}

void APIGateway::ProductGateway::AddProduct(const httplib::Request& req, httplib::Response& res) {
	nlohmann::json json = nlohmann::json::parse(req.body);
	std::optional<Models::Product> product = ToObject<Models::Product>(json);
	if (!product.has_value()) {
		res.status = httplib::BadRequest_400;
		return;
	}
	_productService->ProcessProduct(product.value());
	res.status = httplib::Created_201;
}

void APIGateway::ProductGateway::GetProductById(const httplib::Request& req, httplib::Response& res) {
	std::string idStr = req.path_params.at("id");
	try {
		int id = std::stoi(idStr);
		std::optional<Models::Product> product = _productService->GetProductById(id);
		if (!product.has_value()) {
			res.status = httplib::NotFound_404;
			return;
		}
		std::string jsonResult = ToJson(product.value());
		res.status = httplib::OK_200;
		res.set_content(jsonResult, json_content_type);
	}
	catch (const std::invalid_argument) {
		res.status = httplib::BadRequest_400;
	}
}

void APIGateway::ProductGateway::GetAllProducts(const httplib::Request& req, httplib::Response& res) {
	std::vector<Models::Product> products = _productService->GetAllProducts();
	if (products.size() == 0) {
		res.status = httplib::OK_200;
		res.set_content("[]", json_content_type);
		return;
	}

	std::string jsonRes = ToJson(products);
	res.status = httplib::OK_200;
	res.set_content(jsonRes, json_content_type);
}

void APIGateway::ProductGateway::DeleteProduct(const httplib::Request& req, httplib::Response& res) {
	std::string idStr = req.path_params.at("id");
	try {
		int id = std::stoi(idStr);
		bool isDeleted = _productService->DeleteProduct(id);
		if (isDeleted) {
			res.status = httplib::OK_200;
		}
		else {
			res.status = httplib::NotFound_404;
		}
	}
	catch (const std::invalid_argument) {
		res.status = httplib::BadRequest_400;
	}
}

void APIGateway::ProductGateway::UpdateProduct(const httplib::Request& req, httplib::Response& res) {
	std::string idStr = req.path_params.at("id");
	try {
		int id = std::stoi(idStr);
		nlohmann::json postObject = nlohmann::json::parse(req.body);
		std::optional<Models::Product> product = ToObject<Models::Product>(postObject);

		if (!product.has_value()) {
			res.status = httplib::BadRequest_400;
			return;
		}

		Models::Product updatedProduct = product.value();
		if (id != updatedProduct.Id) {
			res.status = httplib::NotAcceptable_406;
			return;
		}

		bool isUpdated = _productService->UpdateProduct(updatedProduct);
		if (isUpdated) {
			res.status = httplib::OK_200;
			return;
		}
		res.status = httplib::NotFound_404;
	}
	catch (const std::invalid_argument) {
		res.status = httplib::BadRequest_400;
	}
}

void APIGateway::ProductGateway::MapMethods(httplib::SSLServer& server) {
	server.Get("/products", [this](const httplib::Request& req, httplib::Response& res) { GetAllProducts(req, res); });
	server.Get("/product/:id", [this](const httplib::Request& req, httplib::Response& res) { GetProductById(req, res); });
	server.Post("/product", [this](const httplib::Request& req, httplib::Response& res) {AddProduct(req, res); });
	server.Delete("/product/:id", [this](const httplib::Request& req, httplib::Response& res) { DeleteProduct(req, res); });
	server.Put("/product/:id", [this](const httplib::Request& req, httplib::Response& res) { UpdateProduct(req, res); });
}
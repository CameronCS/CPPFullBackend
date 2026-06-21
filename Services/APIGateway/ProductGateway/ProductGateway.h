#ifndef _____PRODUCT_GATEWAY_H_____
#define _____PRODUCT_GATEWAY_H_____

#include <iostream>
#include <httplib.h>
#include <IProductService.h>
#include <Logging/ILogger.h>
#include <Gateway/GatewayBase.h>

namespace APIGateway {
	class ProductGateway : public SystemFramework::Gateway::GatewayBase {
	public:
		ProductGateway(httplib::SSLServer& server, BusinessService::Interface::IProductService* productService, SystemFramework::Logging::ILogger* logger) {
			this->_productService = productService;
			this->_logger = logger;
			logger->SetServiceLevel("API", "Product Gateway");
			MapMethods(server);
		}

	private:
		BusinessService::Interface::IProductService* _productService;
		SystemFramework::Logging::ILogger* _logger;
		void MapMethods(httplib::SSLServer& server);
		void AddProduct(const httplib::Request& req, httplib::Response& res);
		void GetAllProducts(const httplib::Request& req, httplib::Response& res);
		void GetProductById(const httplib::Request& req, httplib::Response& res);
		void DeleteProduct(const httplib::Request& req, httplib::Response& res);
		void UpdateProduct(const httplib::Request& req, httplib::Response& res);
	};
}

#endif